.PHONY: all clean all

debug:
	g++ -fdiagnostics-color=auto -std=c++11 -g src/core.cpp src/block.cpp -o src/coil

all: 
	g++ -fdiagnostics-color=auto -std=c++11 -o2 src/core.cpp src/block.cpp -o src/coil

clean:
	rm -f src/coil
