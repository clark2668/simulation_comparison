module load GNU/4.8.3

which=$1

if [ -z "$which" ]
then
	echo "activate normal cvmfs shell"
	source /cvmfs/ara.opensciencegrid.org/trunk/centos7/setup.sh 
else
	echo "activating shell to allow for custom arasim and araroot"
	export ARA_SETUP_DIR="/cvmfs/ara.opensciencegrid.org/trunk/centos7"
	# export ARA_UTIL_INSTALL_DIR="${ARA_SETUP_DIR%/}/ara_build"
	export ARA_DEPS_INSTALL_DIR="${ARA_SETUP_DIR%/}/misc_build"
	# export ARA_ROOT_DIR="${ARA_SETUP_DIR%/}/source/AraRoot"

	export LD_LIBRARY_PATH="$ARA_UTIL_INSTALL_DIR/lib:$ARA_DEPS_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
	# export DYLD_LIBRARY_PATH="$ARA_UTIL_INSTALL_DIR/lib:$ARA_DEPS_INSTALL_DIR/lib:$DYLD_LIBRARY_PATH"
	export PATH="$ARA_UTIL_INSTALL_DIR/bin:$ARA_DEPS_INSTALL_DIR/bin:$PATH"

	# Run thisroot.sh using `.` instead of `source` to improve POSIX compatibility
	. "${ARA_SETUP_DIR%/}/root_build/bin/thisroot.sh"

	export SQLITE_ROOT="$ARA_DEPS_INSTALL_DIR"
	export GSL_ROOT="$ARA_DEPS_INSTALL_DIR"
	#export FFTW_LIBRARIES="$ARA_DEPS_INSTALL_DIR"
	export FFTWSYS="$ARA_DEPS_INSTALL_DIR"

	export BOOST_ROOT="$ARA_DEPS_INSTALL_DIR/include"
	#export BOOST_LIB="$ARA_DEPS_INSTALL_DIR/lib"
	#export LD_LIBRARY_PATH="$BOOST_LIB:$LD_LIBRARY_PATH"
	#export DYLD_LIBRARY_PATH="$BOOST_LIB:$DYLD_LIBRARY_PATH"

	export ARASIM_PATH=/mnt/home/baclark/ara/sim_comparison/simcom_AraSim
	export LD_LIBRARY_PATH="$ARASIM_PATH:$LD_LIBRARY_PATH"

	export CMAKE_PREFIX_PATH="$ARA_DEPS_INSTALL_DIR"
fi
