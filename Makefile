# Compiler Stuff
Compiler=g++
C++Version=17
CompilerFLAGS=-Wall -Wextra -std=c++$(C++Version)
LDFLAGS=-lncurses -lsndfile -lportaudio
# File Stuff
SRC=src/main.cpp\
		src/ui/screen.cpp\
		src/core/audio_engine.cpp\
		src/core/audio.cpp
OutputDIR=bin/sizzlefx

SRC_TST=tst/main.cpp\
        tst/core/audio.cpp
TSTOutputDIR=bin/sizzlefx.tst

SRC_TST1=tst/wav_creator.cpp
TSTOutputDIR1=bin/sizzlefx.tst1

SRC_TST2=tst/play_audio.cpp
TSTOutputDIR2=bin/sizzlefx.tst2

all:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC) -o $(OutputDIR) $(LDFLAGS)

test:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST) -o $(TSTOutputDIR) $(LDFLAGS)

test1:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST1) -o $(TSTOutputDIR1) $(LDFLAGS)

test2:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST2) -o $(TSTOutputDIR2) $(LDFLAGS)

clean:
	rm -f $(OutputDIR) $(TSTOutputDIR) $(TSTOutputDIR1) $(TSTOutputDIR2)
