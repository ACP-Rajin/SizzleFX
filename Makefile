# Compiler Stuff
Compiler=g++
C++Version=17
CompilerFLAGS=-Wall -Wextra -std=c++$(C++Version)
LDFLAGS=-lncurses -lsndfile -lportaudio

# SRC
SRC=src/main.cpp
OutputDIR=bin/sizzlefx

# TESTS
SRC_TST=test/audio_playback.cpp
TSTOutputDIR=bin/sizzlefx-audio-playback.tst

SRC_TST1=test/rectangle_movement.cpp
TSTOutputDIR1=bin/sizzlefx-rectangle-movement.tst

SRC_TST2=test/wav_creator.cpp
TSTOutputDIR2=bin/sizzlefx-wav-creator.tst

SRC_TST3=test/audio_metadata.cpp
TSTOutputDIR3=bin/sizzlefx-audio-metadata.tst

all:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC) -o $(OutputDIR) $(LDFLAGS)

test0:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST) -o $(TSTOutputDIR) $(LDFLAGS)

test1:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST1) -o $(TSTOutputDIR1) $(LDFLAGS)

test2:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST2) -o $(TSTOutputDIR2) $(LDFLAGS)

test3:
	mkdir -p bin
	$(Compiler) $(CompilerFLAGS) $(SRC_TST3) -o $(TSTOutputDIR3) $(LDFLAGS)

clean:
	rm -f $(OutputDIR) $(TSTOutputDIR) $(TSTOutputDIR1) $(TSTOutputDIR2)
