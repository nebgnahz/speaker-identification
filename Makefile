all: speaker-recognition mfcc-test

CFLAGS=-I. -std=c++11
LIBSNDFILE=`pkg-config --cflags --libs sndfile`

mfcc.o: mfcc.cc
	$(CXX) -c $< -o $@ $(CFLAGS)

mfcc-test: mfcc.o mfcc-test.cc
	$(CXX) $^ -o $@ -lgrt $(CFLAGS)

wav-reader.o: wav-reader.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

speaker-recognition: mfcc.o wav-reader.o speaker-recognition.cpp
	$(CXX) $^ -o $@ -lgrt $(CFLAGS) $(LIBSNDFILE)

test: mfcc-test
	@./mfcc-test test

clean:
	$(RM) mfcc-test speaker-recognition

.PHONY: clean test
