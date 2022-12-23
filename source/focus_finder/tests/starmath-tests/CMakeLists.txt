# Target name
set(target starmath-test)
message(STATUS "Test ${target}")

# Comment the following lines to disable CImg image debug display
# NOTE: Enabling this makes github build fail due to X11 deps.
# TODO: Make this dependent on general debug switch?
find_package (X11 REQUIRED)
add_compile_definitions(DEBUG_IMAGE_DISPLAY_SWITCH)

set(sources
        starmath_test_module.cpp
		point_tests.cpp
		snr_tests.cpp
		centroid_tests.cpp
		hfd_tests.cpp
		star_cluster_algorithm_tests.cpp
        single_star_detector_tests.cpp
        curve_fitting_tests.cpp
		image_slicer_tests.cpp
		../../common/point.cpp
        ../../common/centroid.cpp
		../../common/centroid_algorithm_factory.cpp
		../../common/intensity_weighted_centroid_algorithm.cpp
		../../common/center_of_gravity_centroid_algorithm.cpp
		../../common/hfd.cpp
		../../common/fwhm.cpp
        ../../common/snr.cpp
		../../common/curve_parms.cpp
		../../common/curve_parm.cpp
		../../common/curve_fit_parms.cpp
		../../common/curve_fit_summary.cpp
		../../common/outlier_filter.cpp
		../../common//point_with_residual.cpp
		../../common/lm_curve_matcher_summary.cpp
		../../common/lm_fitting_curve.cpp
		../../common/lm_fitting_curve_factory.cpp
		../../common/lm_fitting_curve_gaussian.cpp
		../../common/lm_fitting_curve_hyperbolic.cpp
		../../common/lm_fitting_curve_hyperbolic_pos_only.cpp
		../../common/lm_fitting_curve_parabolic.cpp
		../../common/lm_curve_matcher.cpp
        ../../common/single_star_detector_algorithm.cpp
        ../../common/star_cluster_algorithm.cpp
        ../../common/thresholding_algorithm_factory.cpp
        ../../common/max_entropy_thresholding_algorithm.cpp
        ../../common/otsu_thresholding_algorithm.cpp
		../../common/mean_thresholding_algorithm.cpp
        star_cluster_algorithm_tests.cpp image_slicer_tests.cpp point_tests.cpp hfd_tests.cpp centroid_tests.cpp image_tests.cpp thresholder_tests.cpp real_world_star_image_tests.cpp astro_image_processing_pipeline_tests.cpp ../../common/include/pipeline/view/images.h ../../common/include/pipeline/view/subtract_background.h ../../common/include/pipeline/view/scale.h ../../common/include/pipeline/view/center_on_star.h ../../common/include/pipeline/view/crop.h ../../common/include/pipeline/action/average.h ../../common/include/pipeline/view/subtract.h ../../common/include/pipeline/action/files.h pipeline_files_tests.cpp pipeline_images_tests.cpp pipeline_subtract_background_tests.cpp pipeline_crop_tests.cpp pipeline_center_on_star_tests.cpp pipeline_scale_tests.cpp pipeline_average_tests.cpp pipeline_subtract_tests.cpp pipeline_divide_by_tests.cpp rect_tests.cpp pipeline_add_tests.cpp pipeline_multiply_with_tests.cpp ../../common/include/pipeline/view/add.h ../../common/include/pipeline/view/multiply_by.h pipeline_multiply_by_tests.cpp pipeline_real_world_tests.cpp)

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
		${GSL_LIBRARY}
		${GSL_CBLAS_LIBRARY}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        ${Boost_LOG_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${DEFAULT_LINKER_OPTIONS}
		${TIFF_LIBRARIES}
		X11
		)

# now register the executable with CTest
add_test(NAME ${target} COMMAND ${target} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
