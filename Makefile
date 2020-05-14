LDFLAGS= -L${shell root-config --libdir} -L${ARA_UTIL_INSTALL_DIR}/lib
CXXFLAGS= -I${shell root-config --cflags} -I${ARA_UTIL_INSTALL_DIR}/include -I${ARA_UTIL_INSTALL_DIR}/../source/AraSim
LDLIBS += $(shell root-config --libs) -lRootFftwWrapper -lAra

