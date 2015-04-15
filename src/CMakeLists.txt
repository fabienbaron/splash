cmake_minimum_required(VERSION 2.8)
project(splash CXX)

set(CMAKE_BUILD_TYPE release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -march=native -mfpmath=sse -flto -ftree-vectorize -ggdb -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -fno-omit-frame-pointer -O2") #Note: valgrind chokes on march=native for some platforms

file(GLOB SOURCE *.h *.cpp)

# Now add the binary
add_executable(splash ${SOURCE})

target_link_libraries(splash opencv_core opencv_highgui opencv_imgproc cfitsio OpenCL)   


