////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////	plotSignal.cpp
////	plot the signal as generated by AraSim
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
#include "TAxis.h"

//AraSim includes
#include "Settings.h"
#include "Primaries.h"
#include "IceModel.h"
#include "Detector.h"
#include "Spectra.h"
#include "secondaries.hh"
#include "Event.h"
#include "signal.hh"
#include "signal.hh"

// #include "FFTtools.h"

using namespace std;

Event *makeNewEvent(Settings *settings, Spectra *spectra, Primaries *primary, IceModel *icemodel, Detector *detector, Signal *signal, Secondaries *sec, int inu_thrown){

	Event *event = new Event();
	settings->EXPONENT = settings->PNU[inu_thrown];
	spectra = new Spectra(settings->EXPONENT);
	settings->SELECT_FLAVOR = settings->NUFLAVORINT[inu_thrown];
	settings->NU_NUBAR_SELECT_MODE = settings->NUBAR[inu_thrown];
	settings->SELECT_CURRENT = settings->CURRENTINT[inu_thrown];
	settings->INTERACTION_MODE = 2;
	settings->POSNU_R = settings->IND_POSNU_R[inu_thrown];
	settings->POSNU_THETA = settings->IND_POSNU_THETA[inu_thrown];
	settings->POSNU_PHI = settings->IND_POSNU_PHI[inu_thrown];
	settings->NNU_THIS_THETA = 1;
	settings->NNU_D_THETA = 0.0;
	settings->NNU_THETA = settings->IND_NNU_THETA[inu_thrown];
	settings->NNU_THIS_PHI = 1;
	settings->NNU_D_PHI = 0.0;
	settings->NNU_PHI = settings->IND_NNU_PHI[inu_thrown];
	settings->YPARAM = 2;
	settings->ELAST_Y = settings->ELAST[inu_thrown];
	// double pnu = spectra->GetNuEnergy();
	event->pnu = spectra->GetNuEnergy();

	string nuflavor = primary->GetNuFlavor(settings);
	int nu_nubar = primary->GetNuNuBar(nuflavor, settings);

	int nuflavorint;

    if (nuflavor=="nue"){
        nuflavorint=1;
    }
    else if (nuflavor=="numu"){
        nuflavorint=2;
    }
    else if (nuflavor=="nutau"){
        nuflavorint=3;
    }

	Interaction *Nu_temp;
	int n_interactions =1;
	Nu_temp = new Interaction (event->pnu, nuflavor, nu_nubar, n_interactions, icemodel, detector, settings, primary, signal, sec );

	event->Nu_Interaction.push_back(*Nu_temp);

	// std::vector<Interaction> Nu_Interaction;
	// Nu_Interaction.push_back(*Nu_temp);
	// delete Nu_temp;
	// cout<<"Inside this function, elast_y is "<<Nu_Interaction[0].elast_y<<endl;

	return event;
}

int main(int argc, char **argv)
{

	// run like "./plotSignal"

	double const ch_angle=56*(PI/180);

	Settings *settings = new Settings();
	string setupfile = "./setup.txt";
	settings->ReadFile(setupfile);
	settings->EVENT_GENERATION_MODE=0;
	// cout<<"event generation mode is "<<settings->EVENT_GENERATION_MODE<<endl;
	string evtfile = "event_list.txt";
	settings->ReadEvtFile(evtfile);
	// for (int i = 0; i < settings1->NNU; i++){
 // 		cout << settings1->EVID[i] << "    " << settings1->NUFLAVORINT[i] << "    " << settings1->NUBAR[i] << "    " << settings1->PNU[i] << "    " << settings1->CURRENTINT[i] << "    " << settings1->IND_POSNU_R[i] << "    " << settings1->IND_POSNU_THETA[i] << "    " << settings1->IND_POSNU_PHI[i] << "    " << settings1->IND_NNU_THETA[i] << "    " << settings1->IND_NNU_PHI[i] << "    " << settings1->ELAST[i] << endl;
	// }

	// settings->SHOWER_MODE=2;

	Primaries *primary = new Primaries();
	IceModel *icemodel=new IceModel(settings->ICE_MODEL + settings->NOFZ*10, 
									settings->CONSTANTICETHICKNESS * 1000 + settings->CONSTANTCRUST * 100 + settings->FIXEDELEVATION * 10 + 0,
									settings->MOOREBAY);// creates Antarctica ice model
	Detector *detector = new Detector(settings, icemodel, setupfile);
	Spectra *spectra;
	// Spectra *spectra = new Spectra(settings->EXPONENT);
	Secondaries *sec = new Secondaries(settings);
	Signal *signal = new Signal(settings);
		signal->SetMedium(0);
		signal->SetParameterization(0);


	// Event *event = new Event();
	// event = new Event(settings, spectra, primary, icemodel, detector, signal, sec, 1);

	Event *event = makeNewEvent(settings, spectra, primary, icemodel, detector, signal, sec, 1);

	cout<<"Shower mode is "<<settings->SHOWER_MODE<<endl;
	cout<<"Elast_y is "<<event->Nu_Interaction[0].elast_y<<endl;
	cout<<"EM frac is "<<event->Nu_Interaction[0].emfrac<<endl;
	cout<<"Had frac is "<<event->Nu_Interaction[0].hadfrac<<endl;
	cout<<"EM LQ is "<<event->Nu_Interaction[0].EM_LQ<<endl;
	cout<<"Had LQ is "<<event->Nu_Interaction[0].HAD_LQ<<endl;
	cout<<"shower_bin is "<<event->Nu_Interaction[0].HAD_shower_Q_profile.size()<<endl;
	settings->SHOWER_STEP=0.001;
	cout<<"Shower step is "<<settings->SHOWER_STEP<<endl;

	int skip_bins;
	double E_shower = event->pnu*event->Nu_Interaction[0].hadfrac;
	double atten_factor = 1. / 10000.;

	double delta_view = 10;



	int num_bins=64;
	double Tarray[num_bins];
	double Earray[num_bins];
	signal->GetVm_FarField_Tarray(event, settings, ch_angle+delta_view*(PI/180), 
								atten_factor, num_bins, Tarray, Earray, skip_bins );

	cout<<"dT is "<<Tarray[1]-Tarray[0]<<endl;

	// int num_bins_more=256;
	// double Tarray_more[num_bins_more];
	// double Earray_more[num_bins_more];
	// signal->GetVm_FarField_Tarray(event, settings, ch_angle+delta_view*(PI/180), 
	// 							atten_factor, num_bins_more, Tarray_more, Earray_more, skip_bins );

	// int num_bins_moremore=512;
	// double Tarray_moremore[num_bins_moremore];
	// double Earray_moremore[num_bins_moremore];
	// signal->GetVm_FarField_Tarray(event, settings, ch_angle+delta_view*(PI/180), 
	// 							atten_factor, num_bins_moremore, Tarray_moremore, Earray_moremore, skip_bins );

	TGraph *gr = new TGraph(num_bins, Tarray, Earray);
	// TGraph *gr_more = new TGraph(num_bins_more, Tarray_more, Earray_more);
	// TGraph *gr_moremore = new TGraph(num_bins_moremore, Tarray_moremore, Earray_moremore);
	TCanvas *c = new TCanvas("","",1100,850);
	c->Divide(1);
	c->cd(1);
		gr->Draw("ALP");
	// c->cd(2);
	// 	gr_more->Draw("ALP");
	// 		// gr_more->SetLineColor(kRed);
	// 		// gr_more->SetLineStyle(9);
	// 	char title[200];
	// 	sprintf(title,"Num bins %d",num_bins_more);
	// 	gr_more->SetTitle(title);
	// c->cd(3);
	// 	gr_moremore->Draw("ALP");
	// 	char title2[200];
	// 	sprintf(title2,"Num bins %d",num_bins_moremore);
	// 	gr_moremore->SetTitle(title2);	

	// gPad->SetLogy();
	// gr->GetYaxis()->SetRangeUser(0.1,3e9);
	gr->SetTitle("64 bins; Time (some units); Amplitude (some units)");
	c->SaveAs("efield_vs_time.png");

}//close the main program
