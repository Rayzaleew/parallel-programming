CXX=g++
LDFLAGS=-I /usr/local/cuda/include/ -L /usr/local/cuda/lib64/ -l OpenCL -std=c++11


.PHONY: all build clean rebuild

all: build

build: 
	g++ main.cpp -fopenmp $(LDFLAGS) -o build

clean:
	rm build
