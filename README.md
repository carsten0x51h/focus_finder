TODO: FOCUS_FINDER
FoFi - FocusFinder - kleiner Professor dreht an Kurben und sitzt auf Teleskop..


----------------------------------------------------------------------------------------------------------------------------

astro_tools
===========

TODO

Helpful astronomy tools.


The idea of astro_tools came up when certain tasks during my
astrophotography sessions came up again and again. Since I am using
Linux, the support for certain tools was not yet as good as for windows.
One thing which was especially interesting was a tool that automatically
finds the best focus position of the camera plugged to the telescope.
Furthermore I wanted to have a kind of sequencer where I can script the
different tasks which are to be done during the night. I liked the idea
of having an automatic re-focusing from time to time - for example
between each frame or when the temperature has changed significantly.
In addition, certain actions should also be available via a commandline
interface.
Later I plan do add a Qt and/or web based GUI for this program.
To control the astronomical devices I use the INDI library.
For image processing I use CCfits and CIMG.


----------------------------------------------------------------------------------------------------------------------------

C++ INDI client library

The library wraps some of the libindi client functionality and extends it by the following features:
   * TODO
   *
   *

Known problems
--------------

-Compiling with clang-3.8 results in an undefined reference error (but works with gcc-6.3):
	  indi_device_test.C:81: undefined reference to `libindiclientpp::VecPropT<libindiclientpp::SwitchTraitsT>
	  libindiclientpp::IndiDeviceT::getVecProp<libindiclientpp::SwitchTraitsT>(std::__cxx11::basic_string<char,
	  std::char_traits<char>, std::allocator<char> > const&)'


The following directories exist in the project
  * TODO


Development & Build & Install

github:  TODO: clone...


build & install
---------------

	1. Go to the build directory (should be empty):

	cd libindiclient++/build


	2. Call cmake .. from the build directory to generate the build environment for your operating system.
	   The parameter -D allows passing parameters to cmake. They are completely optional.
	   Below is the lost of the supported options. The first one is always the _default_.

	   In order to set the desired compiler use your environment variables - for clang e.g.
	   
	   export CC=clang
	   export CXX=clang++

	   NOTE: You can use clang "scan-build" in front of the cmake call to perform static code analysis during the build.
	   	 In this case the environmental variables CCC_CC and CCC_CXX can be set to tell the ccc-analyzer which compiler
		 should actually be used to build the source (see here: http://btorpey.github.io/blog/2015/04/27/static-analysis-with-clang/).
		 
	   cmake .. [-DOPTION_BUILD_DOC = OFF | ON]
	   	    [-DOPTION_BUILD_TESTS = ON | OFF]
		    [-DOPTION_BUILD_EXAMPLES = OFF | ON]
		    [-DBUILD_SHARED_LIBS = ON | OFF]
	            [-DCMAKE_BUILD_TYPE = RELEASE | DEBUG | COVERAGE | PERF]
		    [-DCMAKE_VERBOSE_MAKEFILE = OFF | ON]
		    [-DOPTION_ENABLE_CLANG_TIDY = ON | OFF]
		    [-DCMAKE_CXX_CLANG_TIDY = "clang-tidy;-checks=-*,readability-*"]
		    [-DCMAKE_C_COMPILER = clang | gcc]
		    [-DCMAKE_CXX_COMPILER = clang++ | g++]
		 
	   Examples:

	      a. Build with debug flags displaying make commands. Furthermore use clang++-7 as C++ compiler and clang-7 as C compiler:
	
	            cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_CXX_COMPILER=clang++-7 -DCMAKE_C_COMPILER=clang-7

		    or
		    
		    cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_C_COMPILER=gcc-9

	      b. Or, build with "perf" measurment flags

		    cmake .. -DCMAKE_BUILD_TYPE=PERF


	      c. Or, build with with examples using g++/gcc
		    cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DOPTION_BUILD_EXAMPLES=ON -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

	3. Build the project.
	
		cmake --build . -- [all|clean|test|doc]

		or

		scan-build cmake --build . -- [all|clean|test|doc]

	4. Run the tests manually (optional)
	       export LSAN_OPTIONS=suppressions=../suppr.txt
	       export ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer
	       
	       cd build
	       cmake --build . -- test
	       
	       TODO: Add LSAN_OPTIONS=suppressions=../suppr.txt somehow to CMakeLists.txt.....
	       

	5. Install
	
	       TODO: make install


	6. Create packages (optional)
	       cd build
	       cmake --build . -- pack
	       

Running examples
----------------

	1. Start indiserver with filter wheel simulator:

	indiserver -vv /usr/bin/indi_simulator_wheel /usr/bin/indi_simulator_telescope /usr/bin/indi_simulator_ccd /usr/bin/indi_simulator_focus


	2. Run the filter wheel client - e.g. from the build directory:

	./filter_wheel_cmdd --device="Filter Simulator" --set=3


