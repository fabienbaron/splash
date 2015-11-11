cmake_minimum_required(VERSION 2.8)
project(splash-unittests CXX)

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenCV REQUIRED core imgproc)
find_package(CFITSIO REQUIRED)
find_package(UnitTestPlusPlus REQUIRED)

include_directories(${UnitTestPlusPlus_INCLUDE_DIR})

file(GLOB SOURCE *.h *.cpp)
set(BIN splash-unittests)

add_executable(${BIN} ${SOURCE})

target_link_libraries(${BIN} splashlib ${UnitTestPlusPlus_LIBRARY})

add_custom_target(check ALL "${EXECUTABLE_OUTPUT_PATH}/${BIN}" DEPENDS ${BIN} COMMENT "Executing unit tests...")
add_custom_target(test "${EXECUTABLE_OUTPUT_PATH}/${BIN}" DEPENDS ${BIN} COMMENT "Executing unit tests...")
