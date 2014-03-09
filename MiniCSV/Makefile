# Project: MiniCSV

#CPP      = clang++ -stdlib=libstdc++ -lstdc++ -std=c++11
CPP      = g++ -lstdc++ -std=c++11
CC       = gcc
OBJ      = example.o $(RES)
LINKOBJ  = example.o $(RES)
BIN      = example
CXXFLAGS = -Wall -g -O1 
CFLAGS   = -Wall -g -O1  
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before example all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "example" $(LIBS)

example.o: example.cpp
	$(CPP) -c example.cpp -o example.o $(CXXFLAGS)
