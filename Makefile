test_examples: examples/examples.cc include/lodash/lodash.h
	g++ $< -I ./include/ -std=c++17 -o $@

all: test_examples

clean:
	rm -rf test_examples
