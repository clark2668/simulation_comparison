////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////	plotShowerProfile.cpp
////	plot the shower profile
////////////////////////////////////////////////////////////////////////////////

// C/C++ Includes
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <algorithm>

//ROOT Includes
#include "TGraph.h"
#include "TCanvas.h"
#include "Settings.h"

//AraSim includes
#include "IceModel.h"
#include "signal.hh"

using namespace std;

int main(int argc, char **argv)
{

	// run like "./plotShowerProfile"


	Settings *settings = new Settings();
	Signal *signal = new Signal(settings);
	signal->SetMedium(0);

	double shower_E = 1E18;
	int shower_mode =0; // 0=EM, 1=HAD
	double shower_step_m = 0.001; // also settings->SHOWER_STEP
	int param_model = 0; // 0=jaime's fit, 1= carl's fit 

	vector<double> depth;
	vector<double> Q_shower;
	double LQ;


	signal->GetShowerProfile(shower_E,
							shower_mode,
							shower_step_m,
							param_model,
							depth,
							Q_shower,
							LQ);

	printf("Size of depth is %d \n", depth.size());

	// IceModel *ice =new IceModel(0 + 1*10, 0 * 1000 + 0 * 100 + 0 * 10 + 0, 0);
	// double att = ice->GetARAIceAttenuLength(100);

	// vector<double> depths;
	// vector<double> atts;
	// for(double depth=0; depth<3000; depth+=10.){
	// 	depths.push_back(-depth);
	// 	double att = ice->GetARAIceAttenuLength(depth);
	// 	atts.push_back(att);
	// }

	// // save to csv
	// char title_txt[200];
	// sprintf(title_txt,"interp_values.txt");
	// FILE *fout = fopen(title_txt, "a");
	// for(int i=0; i<depths.size(); i++){
	// 	fprintf(fout,"%.4f,%.4f\n",depths[i], atts[i]);
	// }
	// fclose(fout);//close sigmavsfreq.txt file

	// // make plot
	TGraph *gr = new TGraph(depth.size(), &depth[0], &Q_shower[0]);
	TCanvas *c = new TCanvas("","",1100,850);
	gr->Draw("ALP");
	gPad->SetLogy();
	gr->SetTitle("; Depth (m); Shower Profile (some units)");
	c->SaveAs("profile_vs_depth.png");

}//close the main program
