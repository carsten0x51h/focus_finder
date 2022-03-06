focus_finder
============

This project is about an automatic focus finder software for telescopes. It targets the Linux platform in the first run.

The software aims to support the amateur astronomer (and especially astrophotographer) with one of the most critical but also most annoying tasks: Finding the best focus position for the camera. The main goal is to provide a free and easy to use software that just does the job - automatically.

With a given configuration it should also be possible to execute "FoFi" from the command-line without requiring any user interaction. This way you can include a call to the Focus Finder into a script. This might be useful if the entire observation process should be automatized and you want to re-focus from time to time to compensate the temperature drift. 

When starting this project there was the following situation: There was a free piece of software for automatically finding the "best" focus of the telescope: FocusMax. Steve Brady and Larry Weber developed it and offered it to the public in 2001. Meanwhile there is version 4 of this software available but as far as I was able to see it is no longer free. Instead CCDWare now offers it.

Without understatement this project currently is still in the proof-of-concept phase. That means it is not yet usable. However, if you think this project holds some potential and may be useful for you, or if you maybe work on a similar project, I would be happy to hear your feedback. We might join our efforts and share our experiences and of course any support in the development of this tool is also very welcome!

Please visit my blog https://www.lost-infinity.com for further details. The first article I published about this project can be found here: https://www.lost-infinity.com/fofi-a-free-automatic-telescope-focus-finder-software


build & install
---------------

0. Dependencies
 * cimg
 * CCFits
 * boost >= ?
 * zlib
 * qtbase5-dev
 * libindi >= 1.8.1-1
 * libnova >= 0.15.0-14
 * libgsl

NOTE: In order to compile, the respective dev packages are required.

TODO: Show min versions for distros 
e.g. qt5 >= 5.12.8 (Ubuntu 20.04 focal)


1. Go to the build directory (should be empty):

cd focus_finder/build


2. Call cmake .. from the build directory to generate the build environment for your operating system.
   The parameter -D allows passing parameters to cmake. They are completely optional.
   Below is the lost of the supported options. The first one is always the _default_.

   In order to set the desired compiler use your environment variables - for clang e.g.
	   
   export CC=clang
   export CXX=clang++

   NOTE: You can use clang "scan-build" in front of the cmake call to perform static code analysis during the build.
   	 In this case the environmental variables CCC_CC and CCC_CXX can be set to tell the ccc-analyzer which 
	 compiler should actually be used to build the source (see here: http://btorpey.github.io/blog/2015/04/27/static-analysis-with-clang/).

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

		cmake --build . -- all


4. Run the tests manually (optional)
	export LSAN_OPTIONS=suppressions=../suppr.txt
	export ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer

	cd build
	cmake --build . -- test
	       

Running focus_finder
--------------------

1. Start indiserver with filter wheel simulator:

indiserver -v /usr/bin/indi_simulator_wheel /usr/bin/indi_simulator_telescope /usr/bin/indi_simulator_ccd /usr/bin/indi_simulator_focus

2. Run focus finder GUI application from the build directory:

./focus_finder_guid
