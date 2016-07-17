all: speaker-recognition mfcc-test cblas-test

UNAME := $(shell uname)

BLAS_LIB=-framework Accelerate

ifeq ($(UNAME), Linux)
    BLAS_LIB=-lblas
endif

CFLAGS=-I. -std=c++11 -g
SNDFILE_INC=`pkg-config --cflags sndfile`
SNDFILE_LIB=`pkg-config --libs sndfile`
GRT_INC=`pkg-config --cflags grt`
GRT_LIB=`pkg-config --libs grt`

mfcc.o: mfcc.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(GRT_INC)

mfcc-test: mfcc.o mfcc-test.cc
	$(CXX) $^ -o $@ $(GRT_INC) $(GRT_LIB) $(CFLAGS) $(BLAS_LIB)

wav-reader.o: wav-reader.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(SNDFILE_INC)

speaker-recognition: mfcc.o wav-reader.o speaker-recognition.cpp
	$(CXX) $^ -o $@ -lgrt $(CFLAGS) $(SNDFILE_LIB) $(GRT_INC) $(GRT_LIB) $(BLAS_LIB)

cblas-test: cblas-test.cpp
	$(CXX) $^ -o cblas-test $(BLAS_LIB)

test: mfcc-test
	@time ./mfcc-test test 100

clean:
	$(RM) mfcc-test speaker-recognition *.o

.PHONY: clean test
