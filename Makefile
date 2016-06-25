all: mfcc

CFLAGS=-I. -std=c++11

mfcc.o: mfcc.cc
	$(CXX) -c $< -o $@ $(CFLAGS)

mfcc: mfcc.o main.cc
	$(CXX) $^ -o $@ -lgrt $(CFLAGS)

test: mfcc
	@./mfcc test

clean:
	$(RM) mfcc

.PHONY: clean test
