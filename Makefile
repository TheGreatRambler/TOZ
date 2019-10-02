CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=build

SRC=$(wildcard source/*.cpp source/GadgetFSApi/*.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
