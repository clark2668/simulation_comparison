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

// double temperature(double z) {
//         return(-51.5 + z*(-4.5319e-3 + 5.822e-6*z));
// }

// double temperature(double z) {
//         return(-51.5 + z*(-4.5319e-3 + 5.822e-6*z));
// }

double temperature(double z){
	double temp = (-51.0696) + (0.00267687 * z) + (-1.59061E-08 * pow(z,2.)) + (1.83415E-09 * pow(z,3.));
	return temp;
}

// read depth in positive value, freq in GHz and return attenuation length (m) at the depth
double GetFreqDepIceAttenuLength(double depth, double freq) {

    double AttenL = 0.0;
    // check if depth is positive value
    if ( depth < 0. ) {// whether above the ice or wrong value!
        cerr<<"depth negative! "<<depth<<endl;
    }
    else {
        //AttenL = Tools::SimpleLinearInterpolation_extend_Single(ARA_IceAtten_bin, ARA_IceAtten_Depth, ARA_IceAtten_Length, depth );
        double t = temperature(depth);
        const double f0=0.0001, f2=3.16;
        const double w0=log(f0), w1=0.0, w2=log(f2), w=log(freq);
        const double b0=-6.74890+t*(0.026709-t*0.000884);
        const double b1=-6.22121-t*(0.070927+t*0.001773);
        const double b2=-4.09468-t*(0.002213+t*0.000332);
        double a,bb;
        if(freq<1.){
                a=(b1*w0-b0*w1)/(w0-w1);
                bb=(b1-b0)/(w1-w0);
        }
        else{
                a=(b2*w1-b1*w2)/(w1-w2);
                bb=(b2-b1)/(w2-w1);
        }
        AttenL = 1./exp(a+bb*w);
    }
    return AttenL;
}


int main(int argc, char **argv)
{

	// run like "./plotAtt"

	IceModel *ice =new IceModel(0 + 1*10, 0 * 1000 + 0 * 100 + 0 * 10 + 0, 0);
	// double att = ice->GetARAIceAttenuLength(100);


	double depth=1000.;
	vector<double> freqs;
	vector<double> atts;
	vector<double> atts_1000;
	for(double freq=0.01; freq<1; freq+=0.01){
		freqs.push_back(freq);
		// double att = ice->GetFreqDepIceAttenuLength(depth, freq);
		atts.push_back(GetFreqDepIceAttenuLength(100, freq));
		atts_1000.push_back(GetFreqDepIceAttenuLength(1000, freq));
		// double att = GetFreqDepIceAttenuLength(, freq);
		// cout<<"Att is "<<att<<endl;
		// atts.push_back(att);
	}

	// save to csv
	char title_txt[200];
	sprintf(title_txt,"interp_values.txt");
	FILE *fout = fopen(title_txt, "a");
	for(int i=0; i<freqs.size(); i++){
		fprintf(fout,"%.4f,%.4f,%.4f\n",freqs[i], atts[i], atts_1000[i]);
	}
	fclose(fout);

	// make plot
	TGraph *gr = new TGraph(atts.size(), &freqs[0], &atts[0]);
	TGraph *gr2 = new TGraph(atts.size(), &freqs[0], &atts_1000[0]);
	TCanvas *c = new TCanvas("","",1100,850);
	gr->Draw("ALP");
	gr2->Draw("sameLP");
	gr2->SetLineColor(kBlue);
	gr->SetTitle("; Freq (GHz); Attenuation Length (m)");
	c->SaveAs("attlenght_vs_freq.png");

}//close the main program
