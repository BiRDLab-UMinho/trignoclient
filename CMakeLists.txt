cmake_minimum_required(VERSION 3.12)
project(trignoclient VERSION 1.0)

############
## Config ##
############

# Doxygen descriptors (requires matching Doxyfile.in)
# set(CMAKE_PROJECT_VERSION "Delsys® Trigno Client Library")  # for CMake<3.12
# set(CMAKE_PROJECT_DESCRIPTION "Delsys® Trigno Client Library")  # for CMake<3.9
set(CMAKE_PROJECT_LOGO doc/logo-delsys.png)
set(CMAKE_PROJECT_BRIEF "C++ library with tools for managing a Trigno® Research+ System, as well as acquiring & processing acquired data.")

## Compile using C++17 standard
add_compile_options(-std=c++17)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
# GCC or Clang compiler flags (verbose warnings)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # add_compile_options(-Wall -Wextra -Wpedantic)
  add_compile_options(-O3)  # optimization default for all build types!
endif()

## option (interactive) to enable/disable doc generation
option(WITH_PLOTTING "Build plotting tools (requires additional dependencies)" ON)

## option (interactive) to enable/disable doc generation
option(BUILD_DOC     "Generate documentation (doxygen)" ON)

## option (interactive) to enable/disable doc generation
option(WITH_EXAMPLES "Builds example code as executables" ON)

## set default build type to Release
## only valid when null string (first call)
## in order to change betwenn build types, add flag explicitely i.e. -DCMAKE_BUILD_TYPE=
## or remove CMakeCache from project root

# if (${CMAKE_BUILD_TYPE} MATCHES "")
#     set(CMAKE_BUILD_TYPE Release)
# endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

##################
## Dependencies ##
##################

## Boost
find_package(Boost REQUIRED COMPONENTS system)

if("${Boost_FIND_VERSION}" VERSION_LESS "1.66.1")
	add_definitions(-DBOOST_LEGACY_VERSION)
	# add_compile_definitions(BOOST_LEGACY_VERSION)
endif()

## Doxygen
find_package(Doxygen)

if (${WITH_PLOTTING})
    # manually set OpenCV's search path -> should not be necessary 
    # set(OpenCV_DIR /usr/share/OpenCV)  ## ubuntu default
    # set(OpenCV_DIR /usr/lib/cmake/opencv4)  ## arch linux opencv4 pkg
    # find package

    find_package(OpenCV REQUIRED)
    message(STATUS "OpenCV headers @ ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
endif()

###########
## Build ##
###########

# header locations (multi-target)
include_directories(${PROJECT_NAME}
    include/${PROJECT_NAME}
)

# plotting headers & sources
if (${WITH_PLOTTING})
    include_directories(
        etc/cv-plot/CvPlot/inc
        ${OpenCV_INCLUDE_DIRS}
    )
endif()

# client library
add_library(${PROJECT_NAME}
    # network
    src/client.cpp
    src/interface.cpp
    src/configuration.cpp
    src/basic_configurator.cpp
    src/basic_data_client.cpp
    src/emg_data_client.cpp
    # src/aux_data_client.cpp
    # tools
    # src/recorder.cpp
    # src/exporter.cpp
    # src/logger.cpp
    # src/basic_protocol.cpp
    # # data structures
    src/sample.cpp
    src/frame.cpp
    src/sequence.cpp
)
# plotting headers & sources
if (${WITH_PLOTTING})
    target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/plotter.cpp)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DWITH_PLOTTING)
endif()
# link to external libraries
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    pthread
)
# plotting library dependency
if (${WITH_PLOTTING})
    target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBRARIES}
    )
endif()


# example targets
if (WITH_EXAMPLES)
    # network example
    add_executable(example_client
        src/examples/example_client.cpp
    )
    target_link_libraries(example_client
        ${PROJECT_NAME}
    )

    # general purpose executable to send queries/commands from TCU
    add_executable(example_query
        src/examples/example_query.cpp
    )
    target_link_libraries(example_query
        ${PROJECT_NAME}
    )
endif()


if(${DOXYGEN_FOUND} AND BUILD_DOC)
    # cf. https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

    # set input and output files
    # generates Doxyfile with source and target paths (cf. 'Doxyfile.in')
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message(STATUS "Adding documentation (doxygen) target")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
elseif (NOT ${DOXYGEN_OUT})
    message(STATUS "Doxygen need to be installed to generate the doxygen documentation")
endif()

#############
## Install ##
#############

#headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")

#libraries
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
