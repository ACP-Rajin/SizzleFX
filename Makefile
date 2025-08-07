# Compiler Stuff
Compiler=g++
CompilerFLAGS=-Wall -Wextra -std=c++17
LDFLAGS=-lncurses -lsndfile
# File Stuff
SRC=src/main.cpp\
		src/ui/screen.cpp\
		src/core/audio_engine.cpp

OutputDIR=bin/sizzlefx

all:
	$(Compiler) $(CompilerFLAGS) $(SRC) -o $(OutputDIR) $(LDFLAGS)

clean:
	rm -f $(OutputDIR)
