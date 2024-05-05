
# function( set_required_build_settings_for_GCC8 )
#     # Always link with libstdc++fs.a when using GCC 8.
#     # Note: This command makes sure that this option comes pretty late on the cmdline.
#     link_libraries( "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:-lstdc++fs>" )
# endfunction()

# set_required_build_settings_for_GCC8()

# 
# External dependencies
#
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(INDI COMPONENTS client REQUIRED)

# Enable automoc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)


# ENABLE CMP0020: Automatically link Qt executables to qtmain target on Windows.
set_policy(CMP0020 NEW)


# 
# Executable name and options
# 

# Target name
set(target focus_finder_gui)

# Exit here if required dependencies are not met
if (NOT Qt5Core_FOUND)
    message(STATUS "Example ${target} skipped: Qt5 not found")
    return()
else ()
    message(STATUS "Example ${target}")
endif ()


# 
# Sources
#
set(sources
        ../common/include/point.h
        ../common/point.cpp
        ../common/include/singleton.h
        ../common/include/event_bus.h
        ../common/include/events/new_frame_event.h
        ../common/include/fofi_project_info.h
        ../common/fofi_project_info.cpp
        ../common/include/focus_controller.h
        ../common/focus_controller.cpp
        ../common/include/self_orientation_result.h
        ../common/self_orientation_result.cpp
        ../common/include/timestamp.h
        ../common/timestamp.cpp
        ../common/include/focus_finder_calibration.h
        ../common/focus_finder_calibration.cpp
        ../common/include/floating_point_equality.h
        ../common/include/lerp.h
        ../common/include/quantile.h
        ../common/include/curve_sector.h
        ../common/include/boundary_location.h
        ../common/include/focus_direction.h
        ../common/include/thresholding_algorithm.h
        ../common/include/thresholding_algorithm_type.h
        ../common/include/otsu_thresholding_algorithm.h
        ../common/otsu_thresholding_algorithm.cpp
        ../common/include/max_entropy_thresholding_algorithm.h
        ../common/max_entropy_thresholding_algorithm.cpp
        ../common/include/thresholding_algorithm_factory.h
        ../common/thresholding_algorithm_factory.cpp
        ../common/include/star_cluster_algorithm.h
        ../common/star_cluster_algorithm.cpp
        ../common/include/focus_measure_type.h
        ../common/include/curve_function.h
        ../common/curve_function.cpp
        ../common/include/curve_function_hyperbolic.h
        ../common/curve_function_hyperbolic.cpp
        ../common/include/curve_function_parabolic.h
        ../common/curve_function_parabolic.cpp
        ../common/include/curve_function_gaussian.h
        ../common/curve_function_gaussian.cpp
        ../common/include/curve_function_factory.h
        ../common/curve_function_factory.cpp
        ../common/include/focus_curve_record.h
        ../common/focus_curve_record.cpp
        ../common/include/focus_curve_record_set.h
        ../common/focus_curve_record_set.cpp
        ../common/include/focus_curve_record_set_container.h
        ../common/focus_curve_record_set_container.cpp
        ../common/include/focus_curve_recorder_logic.h
        ../common/focus_curve_recorder_logic.cpp
        ../common/include/focus_curve.h
        ../common/focus_curve.cpp
        ../common/include/focus_curve_recorder.h
        ../common/include/default_focus_curve_recorder.h
        ../common/default_focus_curve_recorder.cpp
        ../common/include/focus_curve_recorder_type.h
        ../common/include/focus_curve_recorder_factory.h
        ../common/focus_curve_recorder_factory.cpp
        ../common/include/image_slicer.h
        ../common/include/wait_for.h
        ../common/include/reporting_dataset.h
        ../common/reporting_dataset.cpp
        ../common/include/reporting.h
        ../common/reporting.cpp
        ../common/include/indi_version_macro.h
        ../common/include/indi_client.h
        ../common/indi_client.cpp
        ../common/include/indi_device.h
        ../common/indi_device.cpp
        ../common/include/indi_camera_interface.h
        ../common/indi_camera_interface.cpp
        ../common/include/indi_focus_interface.h
        ../common/indi_focus_interface.cpp
        ../common/include/indi_helper.h
        ../common/indi_helper.cpp
        ../common/include/indi_device_manager.h
        ../common/include/device_connection_state.h
        ../common/indi_device_manager.cpp
        ../common/include/fitting_curve_type.h
        ../common/include/curve_fit_parms.h
        ../common/curve_fit_parms.cpp
        ../common/include/curve_fit_summary.h
        ../common/curve_fit_summary.cpp
        ../common/include/point_with_residual.h
        ../common/point_with_residual.cpp
        ../common/include/curve_parm.h
        ../common/curve_parm.cpp
        ../common/include/curve_parms.h
        ../common/curve_parms.cpp
        ../common/include/outlier_filter.h
        ../common/outlier_filter.cpp
        ../common/include/timeout_exception.h
        ../common/include/curve_fit_exception.h
        ../common/include/curve_fit_algorithm.h
        ../common/include/gsl_multi_fit_parms.h
        ../common/include/lm_fitting_curve_factory.h
        ../common/lm_fitting_curve_factory.cpp
        ../common/include/lm_curve_matcher.h
        ../common/lm_curve_matcher.cpp
        ../common/include/lm_curve_matcher_summary.h
        ../common/lm_curve_matcher_summary.cpp
        ../common/include/lm_curve_matcher_exception.h
        ../common/include/lm_fitting_curve.h
        ../common/lm_fitting_curve.cpp
        ../common/include/lm_fitting_curve_hyperbolic.h
        ../common/lm_fitting_curve_hyperbolic.cpp
        ../common/include/lm_fitting_curve_hyperbolic_pos_only.h
        ../common/lm_fitting_curve_hyperbolic_pos_only.cpp
        ../common/include/curve_function_hyperbolic_pos_only.h
        ../common/curve_function_hyperbolic_pos_only.cpp
        ../common/include/lm_fitting_curve_parabolic.h
        ../common/lm_fitting_curve_parabolic.cpp
        ../common/include/lm_fitting_curve_gaussian.h
        ../common/lm_fitting_curve_gaussian.cpp
        ../common/include/logging.h
        ../common/logging.cpp
        ../common/include/snr.h
        ../common/snr.cpp
        ../common/include/hfd.h
        ../common/hfd.cpp
        ../common/include/fwhm.h
        ../common/fwhm.cpp
        ../common/include/star_details.h
        ../common/star_details.cpp
        ../common/include/cimg_fits_io.h
        ../common/cimg_fits_io.cpp
        ../common/include/draw_helper.h
        ../common/draw_helper.cpp
        ../common/include/tuple_printer.h
        ../common/include/exception.h
        ../common/include/star_info.h
        ../common/star_info.cpp
        ../common/include/mapper_function.h
        ../common/include/linear_bw_stretch_mapper_function.h
        ../common/linear_bw_stretch_mapper_function.cpp
        ../common/include/spline_curve_mapper_function.h
        ../common/spline_curve_mapper_function.cpp
        ../common/include/image_converter_16to8.h
        ../common/image_converter_16to8.cpp
        ../common/include/rect.h
        ../common/include/binning.h
        ../common/include/camera_interface.h
        ../common/include/device_interface_type.h
        ../common/include/device.h
        ../common/device.cpp
        ../common/include/camera_interface.h
        ../common/include/filter_interface.h
        ../common/include/focus_interface.h
        ../common/include/frame.h
        ../common/include/enum_helper.h
        ../common/frame.cpp
        ../common/include/centroid.h
        ../common/centroid.cpp
        ../common/include/centroid_algorithm.h
        ../common/include/centroid_view.h
        ../common/centroid_view.cpp
        ../common/include/focus_finder_logic.h
        ../common/focus_finder_logic.cpp
        ../common/include/focus_finder_profile.h
        ../common/focus_finder_profile.cpp
        ../common/include/focus_finder.h
        ../common/include/averaged_focus_curve_focusing_strategy.h
        ../common/average_focus_curve_focusing_strategy.cpp
        ../common/include/task_executor.h
        ../common/include/focus_finder_strategy.h
        ../common/include/focus_finder_factory.h
        ../common/focus_finder_factory.cpp
        ../common/include/profile_manager.h
        ../common/profile_manager.cpp
        ../common/device_manager_factory.cpp
        ../common/fofi_config_manager.cpp
        ../common/global_config_manager.cpp
        ../common/global_focus_finder_config.cpp
        ../common/include/device_manager_factory.h
        ../common/include/device_manager_type.h
        ../common/include/device_manager_type_ptree_translator.h
        ../common/include/fofi_config_manager.h
        ../common/include/global_config_manager.h
        ../common/include/global_focus_finder_config.h
        focus_finder_gui_app.cpp
        include/anim_menu_button.h
        anim_menu_button.cpp
        include/abstract_mapper_panel.h
        include/about_dialog.h
        about_dialog.cpp
        include/main_window.h
        main_window.cpp
        include/selectable_image_widget.h
        selectable_image_widget.cpp
        include/image_viewer_panel.h
        image_viewer_panel.cpp
        include/image_converter_panel.h
        image_converter_panel.cpp
        include/linear_bw_stretch_mapper_panel.h
        linear_bw_stretch_mapper_panel.cpp
        include/spline_curve_mapper_panel.h
        spline_curve_mapper_panel.cpp
        include/manage_device_entry_panel.h
        manage_device_entry_panel.cpp
        include/focus_cntl_panel.h
        focus_cntl_panel.cpp
        include/filter_cntl_panel.h
        filter_cntl_panel.cpp
        include/manage_device_profiles_dialog.h
        manage_device_profiles_dialog.cpp
        include/profile_settings_dialog.h
        profile_settings_dialog.cpp
        include/reporting_viewer_panel.h
        reporting_viewer_panel.cpp
        include/fwhm_view_panel.h
        fwhm_view_panel.cpp
        include/hfd_view_widget.h
        hfd_view_widget.cpp
        include/hfd_view_panel.h
        hfd_view_panel.cpp
        include/fwhm_view_widget.h
        fwhm_view_widget.cpp
        include/fwhm_view_panel.h
        fwhm_view_panel.cpp
        include/focus_curve_view_widget.h
        focus_curve_view_widget.cpp
        include/focus_curve_view_panel.h
        focus_curve_view_panel.cpp
        main_window.ui
        manage_device_profiles_dialog.ui
        manage_device_entry_panel.ui
        image_viewer_panel.ui
        image_converter_panel.ui
        linear_bw_stretch_mapper_panel.ui
        spline_curve_mapper_panel.ui
        focus_cntl_panel.ui
        filter_cntl_panel.ui
        about_dialog.ui
        profile_settings_dialog.ui
        reporting_viewer_panel.ui
        include/focus_curve_recorder_dialog.h
        device_manager_dialog_factory.cpp
        include/device_manager_dialog_factory.h
        focus_curve_recorder_dialog.cpp
        focus_curve_recorder_dialog.ui
        include/focus_curve_recorder_progress_details_panel.h
        focus_curve_recorder_progress_details_panel.cpp
        focus_curve_recorder_progress_details_panel.ui
        include/focus_curve_recorder_point_details_panel.h
        focus_curve_recorder_point_details_panel.cpp
        focus_curve_recorder_point_details_panel.ui
        include/focus_curve_recorder_curve_details_panel.h
        focus_curve_recorder_curve_details_panel.cpp
        focus_curve_recorder_curve_details_panel.ui
        include/focus_curve_recorder_summary_details_panel.h
        focus_curve_recorder_summary_details_panel.cpp
        focus_curve_recorder_summary_details_panel.ui
        include/indi_device_manager_dialog.h
        indi_device_manager_dialog.cpp
        indi_device_manager_dialog.ui
        ../common/fofi_project_info.cpp
        fofi_splash_widget.cpp
        include/fofi_splash_widget.h
        ../common/single_pass_focusing_strategy.cpp
        include/star_details_panel.h
        star_details_panel.cpp
        ../common/star_details.cpp
        ../common/include/singleton.h
        ../common/include/event_bus.h
        ../common/include/events/new_frame_event.h
        ../common/include/single_star_detector_algorithm.h ../common/single_star_detector_algorithm.cpp
        ../common/include/indi_version_macro.h ../common/include/centroid_algorithm.h ../common/include/intensity_weighted_centroid_algorithm.h ../common/intensity_weighted_centroid_algorithm.cpp ../common/include/centroid_algorithm_factory.h ../common/include/centroid_algorithm_type.h ../common/centroid_algorithm_factory.cpp ../common/include/mean_thresholding_algorithm.h ../common/mean_thresholding_algorithm.cpp ../common/include/center_of_gravity_centroid_algorithm.h ../common/center_of_gravity_centroid_algorithm.cpp ../common/include/pipeline/view/read.h ../common/include/pipeline/view/subtract_background.h ../common/include/pipeline/view/scale.h ../common/include/pipeline/view/center_on_star.h ../common/include/star_analysis.h ../common/include/pipeline/view/crop.h ../common/include/pipeline/action/average.h ../common/include/pipeline/view/subtract.h ../common/include/pipeline/view/files.h ../common/include/pipeline/view/divide_by.h ../common/include/pipeline/view/add.h ../common/include/pipeline/view/multiply_by.h ../common/include/pipeline/view/detect_stars.h ../common/include/image_reader.h ../common/image_reader.cpp)


# 
# Create executable
# 

# Build executable
add_executable(${target}
        MACOSX_BUNDLE
        ${sources}
        ../../../resources/res.qrc
        )


# 
# Project options
#
set_target_properties(${target}
        PROPERTIES
        ${DEFAULT_PROJECT_OPTIONS}
        FOLDER "${IDE_FOLDER}"
        )


# 
# Include directories
#
target_include_directories(${target}
        PRIVATE
        ${INDI_INCLUDE_DIR}
        ${DEFAULT_INCLUDE_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}/source/include
        )


# 
# Libraries
# 
#

# Add the fllowing to the section below to add further libs
target_link_libraries(${target}
        PRIVATE
        ${DEFAULT_LIBRARIES}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${Boost_LOG_LIBRARY}
        ${Boost_LOG_SETUP_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
		"$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>"
        ${CFITSIO_LIBRARY}
        ${CCFITS_LIBRARY}
        ${GSL_LIBRARY}
        ${GSL_CBLAS_LIBRARY}
        ${INDI_LIBRARIES}
        ${NOVA_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${DEFAULT_LINKER_OPTIONS}
        ${Boost_REGEX_LIBRARY}
        ${TIFF_LIBRARIES}
        )

#
# Compile definitions
#
target_compile_definitions(${target}
        PRIVATE
        ${DEFAULT_COMPILE_DEFINITIONS}
        )


# 
# Compile options
#
target_compile_options(${target}
        PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        )

# 
# Deployment
#

# Executable
install(TARGETS ${target}
        RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
        BUNDLE DESTINATION ${INSTALL_BIN} COMPONENT examples
        )

# IMPORTANT: Otherwise C++11 is used...
#set_property(TARGET ${target} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
