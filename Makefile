# Compiler Stuff
CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17
LDFLAGS=-lncurses -lsndfile
# File Stuff
SRC=src/main.cpp src/ui/screen.cpp src/core/audio_engine.cpp
OUT=bin/sizzlefx

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
