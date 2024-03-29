.PHONY: debug clean all

debug:
	g++ -std=c++17 -DDEBUG -g -ggdb core.cc -o coil

all: 
	g++ -std=c++17 -O2 core.cpp -o coil

clean:
	rm -f src/coil
