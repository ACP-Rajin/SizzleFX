# Compiler Stuff
Compiler=g++
C++Version=17
CompilerFLAGS=-Wall -Wextra -std=c++$(C++Version)
LDFLAGS=-lncurses -lsndfile
# File Stuff
SRC=src/main.cpp\
		src/ui/screen.cpp\
		src/core/audio_engine.cpp
OutputDIR=bin/sizzlefx

SRC_TST=tst/main.cpp\
        tst/core/test.cpp
TSTOutputDIR=bin/sizzlefx.tst

SRC_TST1=tst/wav_creator.cpp
TSTOutputDIR1=bin/sizzlefx.tst1


all:
	$(Compiler) $(CompilerFLAGS) $(SRC) -o $(OutputDIR) $(LDFLAGS)

test:
	$(Compiler) $(CompilerFLAGS) $(SRC_TST) -o $(TSTOutputDIR) $(LDFLAGS)

test1:
	$(Compiler) $(CompilerFLAGS) $(SRC_TST1) -o $(TSTOutputDIR1) $(LDFLAGS)

clean:
	rm -f $(OutputDIR) $(TSTOutputDIR)
