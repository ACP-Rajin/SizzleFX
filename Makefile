# Compiler Stuff
Compiler=g++
C++Version=17

LIB_NAMES=$(notdir $(wildcard libs/*))
LIB_DIRS=$(addprefix -Llibs/,$(addsuffix /lib,$(LIB_NAMES)))
LIB_FLAGS=$(addprefix -l,$(LIB_NAMES))

INCLUDES=$(addprefix -Ilibs/,$(addsuffix /include,$(LIB_NAMES)))
LDFLAGS=$(LIB_DIRS) $(LIB_FLAGS) -lncurses -lsndfile -lportaudio

DebugCompilerFLAGS=-Wall -Wextra -std=c++$(C++Version) -g
ReleaseCompilerFLAGS=-Wall -Wextra -std=c++$(C++Version) -O2

# SRC
SRC=src/main.cpp
OutputDIR=bin/sizzlefx

# DEBUG
DEBUG_SRC=src/main.cpp
DEBUG_OutputDIR=bin/sizzlefx-debug

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
	$(Compiler) $(DebugCompilerFLAGS) $(INCLUDES) $(DEBUG_SRC) -o $(DEBUG_OutputDIR) $(LDFLAGS)

release:
	mkdir -p bin
	$(Compiler) $(ReleaseCompilerFLAGS) $(INCLUDES) $(SRC) -o $(OutputDIR) $(LDFLAGS)

test0:
	mkdir -p bin
	$(Compiler) $(DebugCompilerFLAGS) $(INCLUDES) $(SRC_TST) -o $(TSTOutputDIR) $(LDFLAGS)

test1:
	mkdir -p bin
	$(Compiler) $(DebugCompilerFLAGS) $(INCLUDES) $(SRC_TST1) -o $(TSTOutputDIR1) $(LDFLAGS)

test2:
	mkdir -p bin
	$(Compiler) $(DebugCompilerFLAGS) $(INCLUDES) $(SRC_TST2) -o $(TSTOutputDIR2) $(LDFLAGS)

test3:
	mkdir -p bin
	$(Compiler) $(DebugCompilerFLAGS) $(INCLUDES) $(SRC_TST3) -o $(TSTOutputDIR3) $(LDFLAGS)

clean:
	rm -f $(OutputDIR) $(DEBUG_OutputDIR) $(TSTOutputDIR) $(TSTOutputDIR1) $(TSTOutputDIR2) $(TSTOutputDIR3)

log:
	@echo "Detected Libs:   $(LIB_NAMES)"
	@echo "Includes:       $(INCLUDES)"
	@echo "LDFLAGS:        $(LDFLAGS)"
