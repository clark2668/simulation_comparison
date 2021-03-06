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
#include "TAxis.h"

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

	double shower_E = 6.08986E18;
	int shower_mode = 1; // 0=EM, 1=HAD
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
	cout<<"Q_shower[0] "<<Q_shower[0]<<endl;

	printf("Size of depth is %d \n", depth.size());
	// // make plot
	TGraph *gr = new TGraph(depth.size(), &depth[0], &Q_shower[0]);
	TCanvas *c = new TCanvas("","",1100,850);
	gr->Draw("ALP");
	gPad->SetLogy();
	gr->GetYaxis()->SetRangeUser(0.1,3e9);
	gr->SetTitle("; Depth (m); Shower Profile (some units)");
	c->SaveAs("profile_vs_depth.png");

}//close the main program
