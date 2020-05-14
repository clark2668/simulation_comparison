Get, plot, and save to csv file the ARA ice attenuation model ever ten meters

first activate environment: `source ../env.sh`

then compile: `make -f ../Makefile plotAtt`

then run: `./plotAtt`

symlink to the AraSim data directory (here from cvmfs) is needed in order for the instantiation of the IceModel to work
