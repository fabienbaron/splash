cmake_minimum_required(VERSION 2.8) 
project(splash)

# If you want to override compiler please uncomment the line below
#SET(CMAKE_CXX_COMPILER pathto/mycompiler)

set(CMAKE_BUILD_TYPE release)

# Set some CMake properties:
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
#include_directories(${CMAKE_SOURCE_DIR}/include)

# Build the main directory, always
add_subdirectory(src)



