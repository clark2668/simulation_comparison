#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>

#include "TMath.h"
#include "TRandom3.h"
#include "TVector.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TH2.h"
#include "TGraph.h"

#include "RaySolver.h"
#include "Position.h"
#include "Settings.h"
#include "Settings.h"
#include "IceModel.h"

using namespace std;

int main(int argc, char **argv)
{

	if(argc<3){
		printf("Incorrect usage! Proper usage is ./toleranceStudy <num_trials> <0/1 for no/yes n(z)>\n");
		return -1;
	}

	TH1D *h1_xmiss[2];
	h1_xmiss[0] = new TH1D("x-miss, direct","h1_xmiss_dir",1000,-0.1,0.1);
	h1_xmiss[1] = new TH1D("x-miss, refl/refr","h1_xmiss_ref",1000,-0.1,0.1);

	TH1D *h1_zmiss[2];
	h1_zmiss[0] = new TH1D("z-miss, direct","h1_zmiss_dir",1000,-20,20);
	h1_zmiss[1] = new TH1D("z-miss, refl/refr","h1_zmiss_ref",1000,-20,20);

	TH2D *h2_vertices = new TH2D("vertices w/ sols", "h2_vertices", 40,0,2,40,-2,0);


	Settings *settings = new Settings();
	settings->NOFZ=atoi(argv[2]); //yes, variable depth index of refraction
	settings -> RAY_TRACE_ICE_MODEL_PARAMS=0; //use AraSim original default

	IceModel *ice =new IceModel(0 + 1*10, 0 * 1000 + 0 * 100 + 0 * 10 + 0, 0);
	RaySolver *raysolver = new RaySolver;

	Position source;
	Position target;

	// set the seed ourselves for repeatablity
	// generally a naughty thing to do, but actually useful here...
	TRandom3 *random = new TRandom3(12345); 


	/*
		"Solve_Ray" is actually used in AraSim
		the function requires that antenna positions be given relative
		to earth center, so we have to make that conversion first
		so we quickly set the source to 0,0,0
		then get the surface height
		and then sink the antenna into the earth by that amount

		Outputs is 5 x 2 vector (at most)
		It goes: 	outputs[0][0] = path length, direct
					outpuss[0][1] = path length, refracted/reflected
					outputs[1][0] = launch angle, direct
					outputs[1][1] = launch angle, refracted/reflected
					outputs[2][0] = receipt angle, direct
					outputs[2][1] = receipt angle, refracted/reflected
					outputs[3][0] = reflection angle, direct
					outputs[3][1] = reflection angle, refracted/reflected
					outputs[4][0] = path time, direct
					outputs[4][1] = path time, refracted/reflected
		RayStep is 2 x 2 x N vector (at most)
		First index is solution, second index is dimension (horizontal vs vertical), third index is step size
		It goes:	RayStep[0][0][step] = direct solution, x dimension, n-th step
					RayStep[0][1][step] = refracted/reflected, z dimension, n-th step
	*/


	target.SetXYZ(0.,0.,0.);
	double offset_depth = ice->Surface(target.Lon(), target.Lat());
	target.SetXYZ(0, 0, -200+offset_depth);

	double range=2000;

	int tries=0;
	int num_tries=int(atof(argv[1]));
	while(tries<=num_tries){
		
		// set random source position in a cylinder
		double thisPhi = random->Rndm() * (2*TMath::Pi());
		double thisR = pow(random->Rndm(), 0.5) * range;
		double thisX = thisR*TMath::Cos(thisPhi);
		double thisY = thisR*TMath::Sin(thisPhi);
		double thisZ = random->Uniform(-range, 0.);

		// printf("%.2f, %.2f, %.2f \n", thisX, thisY, thisZ);

		source.SetXYZ(thisX, thisY, thisZ);
		double offset_depth_source = ice->Surface(source.Lon(), source.Lat());
		source.SetZ(source.GetZ()+offset_depth_source);
		
		// test case brian used to debug
		// source.SetXYZ(1067.95, 822.38, random->Uniform(-3000.,0.)+offset_depth);
		
		// the following transformation rotates the problem into the 2-d x-z plane
		// so source_2d and target_2d will share the same y coordinate
		// solving the problem in 2d is how the ray-tracer will do it too
		// and that's how the path length will be computed, which we'll need later
		Vector diff = target - source;
		Vector source_2d = source;
		Vector target_2d = diff.RotateZ(-diff.Phi()) + source;
		Vector diff_2d = source_2d - target_2d;

		// printf("Source 2d %.2f, %.2f, %.2f \n", source_2d.GetX(), source_2d.GetY(), source_2d.GetZ());
		// printf("Target 2d %.2f, %.2f, %.2f \n", target_2d.GetX(), target_2d.GetY(), target_2d.GetZ());
		// printf("Diff 2d %.2f, %.2f, %.2f \n", diff_2d.GetX(), diff_2d.GetY(), diff_2d.GetZ());

		double goalX = -diff_2d.GetX(); // the ray should reach this X coordinate
		double goalZ = target_2d.GetZ()-offset_depth; // and the depth of the target antenna

		vector < vector <double> > outputs;
		vector < vector < vector <double> > > RayStep;
		raysolver->Solve_Ray(source, target, ice, outputs, settings, RayStep);

		if(outputs.size()>0){

			h2_vertices->Fill(abs(diff_2d.GetX()/1.e3), (source_2d.GetZ()-offset_depth_source)/1.e3); // [m]
			
			int num_solutions = outputs[0].size();

			// if(num_solutions==1) cout<<"Only 1 solution!"<<endl;
			for(int sol=0; sol<num_solutions; sol++){

				// cout<<"Sol "<<sol<<" path length is "<<outputs[4][sol]<<endl;
			
				int num_steps = (int)RayStep[sol][0].size();	

				// figure out how far the ray missed the target by in x and z
				double this_xmiss = goalX - RayStep[sol][0][num_steps-1];
				double this_zmiss = goalZ - RayStep[sol][1][num_steps-1];

				h1_xmiss[sol]->Fill(this_xmiss);
				h1_zmiss[sol]->Fill(this_zmiss);
			}

		}
		tries++;
	}

	// make a bunch of plots
	gStyle->SetOptStat(111111); // show OF, UF, STD, mean, integral, and title
	gStyle->SetPalette(kBird);

	double plotmax = h1_xmiss[0]->GetMaximum();

	TGraph *station = new TGraph();
	station->SetPoint(0,0,-200.);

	TCanvas *c_miss = new TCanvas("c_miss","c_miss",2*1100,850);
	c_miss->Divide(3,1);
	c_miss->cd(1);
		h2_vertices->Draw("colz");
			h2_vertices->SetTitle("vertices w/ solutions; x-distance [km]; z-distance [km]; Number of Vertices");
			h2_vertices->SetStats(0);
		station->Draw("sameP");
			station->SetMarkerStyle(33);
			station->SetMarkerSize(2);
		gPad->SetRightMargin(0.15);
		gPad->SetLeftMargin(0.12);
	c_miss->cd(2);
		h1_xmiss[0]->Draw("hist");
			h1_xmiss[0]->SetTitle("horizontal 'miss' distance; 'miss' distance [m]; Number of Vertices");
			h1_xmiss[0]->SetLineColor(kBlue);
			gPad->SetLogy();
			h1_xmiss[0]->GetYaxis()->SetRangeUser(0.1,num_tries*2);
		h1_xmiss[1]->Draw("histsames");
			h1_xmiss[1]->SetLineColor(kRed);
	c_miss->cd(3);
		h1_zmiss[0]->Draw("hist");
			h1_zmiss[0]->SetTitle("vertical 'miss' distance ; 'miss' distance [m]; Number of Vertices");
			h1_zmiss[0]->SetLineColor(kBlue);
			gPad->SetLogy();
			h1_zmiss[0]->GetYaxis()->SetRangeUser(0.1,num_tries*2);
		h1_zmiss[1]->Draw("histsames");
			h1_zmiss[1]->SetLineColor(kRed);
	c_miss->Update();
	TPaveStats *stats1 = (TPaveStats*)h1_xmiss[1]->FindObject("stats");
		stats1->SetY1NDC(0.20);
		stats1->SetY2NDC(0.45);
		stats1->SetTextColor(kRed);
	TPaveStats *stats2 = (TPaveStats*)h1_zmiss[1]->FindObject("stats");
		stats2->SetY1NDC(0.20);
		stats2->SetY2NDC(0.45);
		stats2->SetTextColor(kRed);

	c_miss->Modified();
	char save_title[150];
	sprintf(save_title,"h1_miss_%dcounts_nofz_%d.png",num_tries,settings->NOFZ);
	c_miss->SaveAs(save_title);

}   //end main