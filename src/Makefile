CC=g++
CFLAGS = -I/usr/include/ -I/usr/include/opencv 
LIBS = -L/usr/lib/x86_64-linux-gnu/ -lopencv_core -lopencv_highgui -lopencv_imgproc -l OpenCL -lcfitsio

SOURCES=fitsregistration.cpp core.cpp host_program_opencl.cpp similarityMetric.cpp mse.cpp psnr.cpp ssim.cpp msssim.cpp iqi.cpp mse_opencl.cpp ssim_opencl.cpp msssim_opencl.cpp iqi_opencl.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=fitsregistration

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
		$(CC) -g -fno-omit-frame-pointer  -o $@  $(OBJECTS) $(LIBS)

$(OBJECTS): $(SOURCES)
		$(CC) -g -fno-omit-frame-pointer $(CFLAGS)  -c $(SOURCES)

clean:
		rm -rf fitsregistration *.o

