
# 
# Executable name and options
# 

# Target name
set(target focus_finder-test)
message(STATUS "Test ${target}")


# 
# Sources
# 

set(sources
  focus_finder_test.C
  test1.C
)


# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})

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
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
    ${INDI_INCLUDE_DIR}
)


# 
# Libraries
# 

#TODO: Remove...
#set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.

find_package(Boost COMPONENTS unit_test_framework program_options REQUIRED)

# For adding libs, add the following to the section below...
#${META_PROJECT_NAME}::libindiclientpp
target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${DEFAULT_LINKER_OPTIONS}
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
# Linker options
# 



# IMPORTANT: Otherwise C++11 is used...
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)
