cmake_minimum_required(VERSION 2.8)
project(splash CXX)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-Ofast -march=native -mfpmath=sse -flto -ftree-vectorize -ggdb -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "-g -fno-omit-frame-pointer -O2") #Note: valgrind chokes on march=native for some platforms

find_package(OpenCV REQUIRED core imgproc)
find_package(CFITSIO REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})
include_directories(${CFITSIO_INCLUDE_DIRS})

file(GLOB SOURCE *.h *.cpp)

# Now add the binary
add_executable(splash ${SOURCE})

target_link_libraries(splash ${OpenCV_LIBS} ${CFITSIO_LIBRARIES})
