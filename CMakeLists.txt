
# 
# CMake options
# 


# CMake version
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Include cmake modules

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(WriterCompilerDetectionHeaderFound NOTFOUND)

include(GenerateExportHeader)
include(ExternalProject)


# Set compiler standard
# See https://stackoverflow.com/questions/40452287/let-cmake-with-clang-use-c11-c14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# This module is only available with CMake >=3.1, so check whether it could be found
include(WriteCompilerDetectionHeader OPTIONAL RESULT_VARIABLE WriterCompilerDetectionHeaderFound)

include(cmake/GetGitRevisionDescription.cmake)
include(cmake/Custom.cmake)

# Set policies
set_policy(CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.


# 
# Project description and (meta) information
# 

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)

# Meta information about the project
set(META_PROJECT_NAME        "fofi")
set(META_PROJECT_DESCRIPTION "A free & automatic telescope focus finder")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "https://github.com/carsten0x51h/")
set(META_AUTHOR_MAINTAINER   "c.schmitt51h@gmail.com")
set(META_VERSION_MAJOR       "1")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")


# 
# Project configuration options
# 

# Project options
option(BUILD_SHARED_LIBS        "Build shared instead of static libraries."              ON)
option(OPTION_SELF_CONTAINED    "Create a self-contained install with all dependencies." OFF)
option(OPTION_BUILD_TESTS       "Build tests."                                           ON)
option(OPTION_BUILD_DOC         "Build documentation."                                   OFF)
option(OPTION_BUILD_EXAMPLES    "Build examples."                                        OFF)
option(OPTION_ENABLE_CLANG_TIDY "Enable clang-tidy."                                     ON)


#
# clang-tidy settings (enabled by default)
#
if(OPTION_ENABLE_CLANG_TIDY)
  set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=-*,readability-*,-readability-magic-numbers,-readability-function-cognitive-complexity")
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,readability-*,-readability-magic-numbers,-readability-function-cognitive-complexity")
endif()


# 
# Declare project
# 

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")  

# Declare project
project(${META_PROJECT_NAME} C CXX)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Create version file
file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${META_NAME_VERSION}")


# 
# Compiler settings and options
# 

include(cmake/CompileOptions.cmake)

#
# Look for INDI
#
find_package(INDI REQUIRED)

find_package(Nova REQUIRED)
include_directories(${NOVA_INCLUDE_DIR})

#
# Look for CCfits
#
include(cmake/FindCCFits.cmake)

#
# Look for GSL
#
include(cmake/FindGSL.cmake)

#
# C++ std filesystem library
#
include(cmake/FindFilesystem.cmake)

#
# Look for BOOST
#
# https://stackoverflow.com/questions/17844085/boost-log-with-cmake-causing-undefined-reference-error
# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.

#find_package (Threads REQUIRED)
#find_package (X11 REQUIRED)
find_package (ZLIB REQUIRED)

find_package(Boost COMPONENTS log system regex thread REQUIRED)

#find_package(Boost 1.73 REQUIRED COMPONENTS log)
#find_library(loglib libboost_log.a 3rdparty/boost_1_73_0/stage/lib)
#target_link_libraries(Boost::log INTERFACE ${loglib})


ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

include(FindTIFF)


# 
# Deployment/installation setup
# 

# Get project name
set(project ${META_PROJECT_NAME})

# Check for system dir install
set(SYSTEM_DIR_INSTALL FALSE)
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr" OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    set(SYSTEM_DIR_INSTALL TRUE)
endif()

# Installation paths
if(UNIX AND SYSTEM_DIR_INSTALL)
    # Install into the system (/usr/bin or /usr/local/bin)
    set(INSTALL_ROOT      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_CMAKE     "share/${project}/cmake") # /usr/[local]/share/<project>/cmake
    set(INSTALL_EXAMPLES  "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_DATA      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_BIN       "bin")                    # /usr/[local]/bin
    set(INSTALL_SHARED    "lib")                    # /usr/[local]/lib
    set(INSTALL_LIB       "lib")                    # /usr/[local]/lib
    set(INSTALL_INCLUDE   "include")                # /usr/[local]/include
    set(INSTALL_DOC       "share/doc/${project}")   # /usr/[local]/share/doc/<project>
    set(INSTALL_SHORTCUTS "share/applications")     # /usr/[local]/share/applications
    set(INSTALL_ICONS     "share/pixmaps")          # /usr/[local]/share/pixmaps
    set(INSTALL_INIT      "/etc/init")              # /etc/init (upstart init scripts)
else()
    # Install into local directory
    set(INSTALL_ROOT      ".")                      # ./
    set(INSTALL_CMAKE     "cmake")                  # ./cmake
    set(INSTALL_EXAMPLES  ".")                      # ./
    set(INSTALL_DATA      ".")                      # ./
    set(INSTALL_BIN       ".")                      # ./
    set(INSTALL_SHARED    "lib")                    # ./lib
    set(INSTALL_LIB       "lib")                    # ./lib
    set(INSTALL_INCLUDE   "include")                # ./include
    set(INSTALL_DOC       "doc")                    # ./doc
    set(INSTALL_SHORTCUTS "misc")                   # ./misc
    set(INSTALL_ICONS     "misc")                   # ./misc
    set(INSTALL_INIT      "misc")                   # ./misc
endif()

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL

if(NOT SYSTEM_DIR_INSTALL)
    # Find libraries relative to binary
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@loader_path/../../../${INSTALL_LIB}")
    else()
        set(CMAKE_INSTALL_RPATH "$ORIGIN/${INSTALL_LIB}")       
    endif()
endif()


# 
# Project modules
# 

add_subdirectory(source)
add_subdirectory(doc)
add_subdirectory(deploy)


# 
# Deployment (global project files)
# 

# Install version file
install(FILES "${PROJECT_BINARY_DIR}/VERSION" DESTINATION ${INSTALL_ROOT} COMPONENT runtime)

# Install cmake find script for the project
install(FILES ${META_PROJECT_NAME}-config.cmake DESTINATION ${INSTALL_ROOT} COMPONENT dev)

# Install the project meta files
install(FILES AUTHORS   DESTINATION ${INSTALL_ROOT} COMPONENT runtime)
install(FILES LICENSE   DESTINATION ${INSTALL_ROOT} COMPONENT runtime)
install(FILES README.md DESTINATION ${INSTALL_ROOT} COMPONENT runtime)

# Install runtime data
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION ${INSTALL_DATA} COMPONENT runtime)









# This section manages the different compile / link options
#SET(CFLAGS_COMMON "-Wall -pedantic -Werror -Wno-error=unused-function -Wno-error=unused-parameter -Wno-unused-function -Wno-unused-parameter -std=c++1z")
#SET(CXXFLAGS_COMMON "-Wall -pedantic -Werror -Wno-error=unused-function -Wno-error=unused-parameter -Wno-unused-function -Wno-unused-parameter -std=c++1z")
SET(CFLAGS_COMMON "-std=c++1z")
SET(CXXFLAGS_COMMON "-std=c++1z")

# Add new build types
message("* Adding build types...")

# Coverage - see http://www.network-theory.co.uk/docs/gccintro/gccintro_81.html
SET(CMAKE_C_FLAGS_COVERAGE
    "${CFLAGS_COMMON} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
SET(CMAKE_CXX_FLAGS_COVERAGE
    "${CXXFLAGS_COMMON} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE )

# An advanced variable will not be displayed in any of the cmake GUIs unless the show advanced option is on.
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE )


# Perf - TODO: should be O3
SET(CMAKE_C_FLAGS_PERF
    "${CFLAGS_COMMON} -O0 -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C compiler during perf builds."
    FORCE )
SET(CMAKE_CXX_FLAGS_PERF
    "${CXXFLAGS_COMMON} -O0 -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C++ compiler during perf builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_PERF
    ""
    CACHE STRING "Flags used for linking binaries during perf builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_PERF
    ""
    CACHE STRING "Flags used by the shared libraries linker during perf builds."
    FORCE )
  
# An advanced variable will not be displayed in any of the cmake GUIs unless the show advanced option is on.
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PERF
    CMAKE_C_FLAGS_PERF
    CMAKE_EXE_LINKER_FLAGS_PERF
    CMAKE_SHARED_LINKER_FLAGS_PERF )


# Debug
set(CMAKE_C_FLAGS_DEBUG
  "${CFLAGS_COMMON} -O0 -g -fsanitize=address -fno-omit-frame-pointer"
  CACHE STRING "Flags used by the C compiler during debug builds."
  FORCE
  )
set(CMAKE_CXX_FLAGS_DEBUG
  "${CXXFLAGS_COMMON} -O0 -g -fsanitize=address -fno-omit-frame-pointer"
  CACHE STRING "Flags used by the C++ compiler during debug builds."
  FORCE
  )


  
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release
    CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage Perf."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
   
message("* Current build type is : ${CMAKE_BUILD_TYPE}")




