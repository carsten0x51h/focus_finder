
# 
# External dependencies
# 

find_package(Qt5Core    5.1)
find_package(Qt5Gui     5.1)
find_package(Qt5Widgets 5.1)

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
else()
    message(STATUS "Example ${target}")
endif()


# 
# Sources
# 

set(sources
	../common/include/quantile.h
	../common/include/focus_finder_record.h
	../common/focus_finder_record.cpp
	../common/include/focus_curve.h
	../common/focus_curve.cpp
	../common/include/image_slicer.h
	../common/include/wait_for.h
	../common/include/indi_driver_info.h
	../common/include/reporting_dataset.h
	../common/reporting_dataset.cpp
	../common/include/reporting.h
	../common/reporting.cpp
	../common/include/indi_client.h
	../common/indi_client.cpp
	../common/include/indi_device_factory.h
	../common/indi_device_factory.cpp
	../common/include/indi_usb_device_connector.h
	../common/indi_usb_device_connector.cpp
	../common/include/indi_camera.h
	../common/indi_camera.cpp
	../common/include/indi_focus.h
	../common/indi_focus.cpp
	../common/include/indi_helper.h
	../common/indi_helper.cpp
	../common/include/indi_device_manager.h
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
	../common/include/camera.h
	../common/include/device_type.h
	../common/include/device.h
	../common/device.cpp
	../common/include/camera.h
	../common/include/cooler.h
	../common/include/filter.h
	../common/include/focus.h
	../common/include/frame.h
	../common/include/enum_helper.h
	../common/frame.cpp
	../common/include/centroid.h
	../common/centroid.cpp
	../common/include/centroid_view.h
	../common/centroid_view.cpp
	../common/include/dummy_device_connector.h
	../common/dummy_device_connector.cpp
	../common/dummy_camera.cpp
	../common/include/dummy_focus.h
	../common/dummy_focus.cpp
	../common/include/dummy_filter.h
	../common/dummy_filter.cpp
	../common/include/device_manager.h
	../common/include/dummy_device_manager.h
	../common/dummy_device_manager.cpp
	../common/include/focus_finder_logic.h
	../common/focus_finder_logic.cpp
	../common/include/focus_finder_profile.h
	../common/focus_finder_profile.cpp
	../common/include/focus_finder.h
	../common/include/focus_finder_fast_curve_lookup.h
	../common/focus_finder_fast_curve_lookup.cpp
	../common/include/focus_finder_executor.h
	../common/focus_finder_executor.cpp
	../common/include/focus_finder_strategy.h
	../common/include/focus_finder_factory.h
	../common/focus_finder_factory.cpp
	../common/include/profile_manager.h
	../common/profile_manager.cpp
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
)


# 
# Create executable
# 

#qt5_add_resources(res.qrc)

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
    ../../../resources/res.qrc
)

# Create namespaced alias
# TODO: Required???
#add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


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

# Add the fllowing to the section below to add further libs
#    ${META_PROJECT_NAME}::libindiclientpp
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
    stdc++fs
    cfitsio
    CCfits
    gsl
    gslcblas
    indi
    indiclient
    z
    ${DEFAULT_LINKER_OPTIONS}
)

#   cfitsio
 
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
# Linker options
#
# target_link_libraries(${target}
#     PRIVATE
#     ${DEFAULT_LINKER_OPTIONS}
# )


# 
# Deployment
#

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT examples
)

# IMPORTANT: Otherwise C++11 is used...
set_property(TARGET ${target} PROPERTY CXX_STANDARD 20)

