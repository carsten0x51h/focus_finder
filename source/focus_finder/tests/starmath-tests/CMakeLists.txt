# Target name
set(target starmath-test)
message(STATUS "Test ${target}")

set(sources
        starmath_test_module.cpp
        single_star_detector_tests.cpp
        ../../common/cimg_fits_io.cpp
        ../../common/centroid.cpp
        ../../common/snr.cpp
        ../../common/single_star_detector_algorithm.cpp
        ../../common/star_cluster_algorithm.cpp
        ../../common/thresholding_algorithm_factory.cpp
        ../../common/max_entropy_thresholding_algorithm.cpp
        ../../common/otsu_thresholding_algorithm.cpp
        )

# create an executable, which instantiates a runner from
# GoogleTest, Boost.Test, QtTest or whatever framework you use
# Build executable
add_executable(${target}
        ${sources}
        )

# depending on the framework, you need to link to it
target_link_libraries(${target}
        PRIVATE
        ${DEFAULT_LIBRARIES}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        ${Boost_LOG_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${CFITSIO_LIBRARY}
        ${CCFITS_LIBRARY}
        ${DEFAULT_LINKER_OPTIONS}
        )

# now register the executable with CTest
add_test(NAME ${target} COMMAND ${target} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
