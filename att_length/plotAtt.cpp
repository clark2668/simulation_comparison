////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////	plotAtt.cpp
////	plot the attenuation
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

//AraSim includes
#include "IceModel.h"

using namespace std;

int main(int argc, char **argv)
{

	// run like "./plotAtt"

	IceModel *ice =new IceModel(0 + 1*10, 0 * 1000 + 0 * 100 + 0 * 10 + 0, 0);
	double att = ice->GetARAIceAttenuLength(100);

	vector<double> depths;
	vector<double> atts;
	for(double depth=0; depth<3000; depth+=10.){
		depths.push_back(-depth);
		double att = ice->GetARAIceAttenuLength(depth);
		atts.push_back(att);
	}

	// save to csv
	char title_txt[200];
	sprintf(title_txt,"interp_values.txt");
	FILE *fout = fopen(title_txt, "a");
	for(int i=0; i<depths.size(); i++){
		fprintf(fout,"%.4f,%.4f\n",depths[i], atts[i]);
	}
	fclose(fout);//close sigmavsfreq.txt file

	// make plot
	TGraph *gr = new TGraph(atts.size(), &atts[0], &depths[0]);
	TCanvas *c = new TCanvas("","",1100,850);
	gr->Draw("ALP");
	gr->SetTitle("; Attenuation Length (m); Depth (m)");
	c->SaveAs("depth_vs_att.png");

}//close the main program
