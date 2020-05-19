#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>

#include "TMath.h"

#include "RaySolver.h"
#include "Position.h"
#include "Settings.h"
#include "Settings.h"
#include "IceModel.h"

using namespace std;

int main(int argc, char **argv)
{

	Settings *settings = new Settings();
	settings->NOFZ=1; //yes, variable depth index of refraction
	settings -> RAY_TRACE_ICE_MODEL_PARAMS=0; //use AraSim original default

	IceModel *ice =new IceModel(0 + 1*10, 0 * 1000 + 0 * 100 + 0 * 10 + 0, 0);
	RaySolver *raysolver = new RaySolver;

	Position source;
	Position target;

	/*
		the first version is the "Solve_Ray_org" version
		which is a good example of how to use the ray tracer

		Outputs is 3 x 2 vector (at most)
		It goes: 	outputs[0][0] = path length, direct
					outpuss[0][1] = path length, refracted/reflected
					outputs[1][0] = launch angle, direct
					outputs[1][1] = launch angle, refracted/reflected
					outputs[2][0] = receipt angle, direct
					outputs[2][1] = receipt angle, refracted/reflected
	*/

	vector < vector <double> > outputs;
	source.SetXYZ(647.72309225,  2946.93430118, -1777.57697991); //also need to reset these variables
	target.SetXYZ(0, 0, -100);
	raysolver->Solve_Ray_org(source, target, outputs, settings);
	if(outputs.size()>0){
		int num_solutions = outputs[0].size();
		printf("Num solutions: %d \n",num_solutions);
		for(int sol=0; sol<num_solutions; sol++){
			printf("Solution %d, path length %.3f, launch angle %.3f, receipt angle %.3f \n",
				sol,
				outputs[0][sol],
				180. - (outputs[1][sol]*TMath::RadToDeg()),
				180. - (outputs[2][sol])*TMath::RadToDeg());
		}
	}


	/*
		the second version is the "Solve_Ray" version
		which is the one actually used in AraSim
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

	source.SetXYZ(0.,0.,0.);
	double offset_depth = ice->Surface(source.Lon(), source.Lat());
	source.SetXYZ(647.72309225,  2946.93430118, -1777.57697991+offset_depth);
	target.SetXYZ(0, 0, -100+offset_depth);

	vector < vector < vector <double> > > RayStep;
	outputs.clear(); // clear this out in preparation for the second "flavor" of ray tracer call
	raysolver->Solve_Ray(source, target, ice, outputs, settings, RayStep);
	if(outputs.size()>0){
		int num_solutions = outputs[0].size();
		printf("Num solutions: %d \n",num_solutions);
		for(int sol=0; sol<num_solutions; sol++){
			// compute the attenuation factor by looping over the ray steps
			double atten_factor=1.;
			for(int step=1; step< (int) RayStep[sol][0].size(); step++){
				double dx = RayStep[sol][0][step - 1] - RayStep[sol][0][step];
				double dz = RayStep[sol][1][step - 1] - RayStep[sol][1][step];
				double dl = sqrt((dx * dx) + (dz * dz));
				atten_factor *= exp(-dl/ice->GetARAIceAttenuLength(-RayStep[sol][1][step]));
			}
			printf("Solution %d, path length %.3f, launch angle %.3f, receipt angle %.3f, atten %.4f \n",
				sol,
				outputs[0][sol],
				180. - (outputs[1][sol]*TMath::RadToDeg()),
				180. - (outputs[2][sol])*TMath::RadToDeg(),
				atten_factor);
		}	
	}

}   //end main