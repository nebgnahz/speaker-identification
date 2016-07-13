all: speaker-recognition mfcc-test cblas-test

CFLAGS=-I. -std=c++11
LIBSNDFILE=`pkg-config --cflags --libs sndfile`
GRT_INC=`pkg-config --cflags grt`
GRT_LIB=`pkg-config --libs grt`

mfcc.o: mfcc.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(GRT_INC)

mfcc-test: mfcc.o mfcc-test.cc
	$(CXX) $^ -o $@ $(GRT_INC) $(GRT_LIB) $(CFLAGS) -framework Accelerate

wav-reader.o: wav-reader.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(LIBSNDFILE)

speaker-recognition: mfcc.o wav-reader.o speaker-recognition.cpp
	$(CXX) $^ -o $@ -lgrt $(CFLAGS) $(LIBSNDFILE) $(GRT_INC) $(GRT_LIB) -framework Accelerate

cblas-test: cblas-test.cpp
	$(CXX) $^ -o cblas-test -framework Accelerate

test: mfcc-test
	@./mfcc-test test

clean:
	$(RM) mfcc-test speaker-recognition *.o

.PHONY: clean test
