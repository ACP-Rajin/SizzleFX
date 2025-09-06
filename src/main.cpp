#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>

#include "ui/screen.hpp"
#include "core/audio.cpp"

int main(int argc,char** argv){
  using namespace SizzleFX;
  // Check for input .wav file
  if(argc<2){
    std::cerr<<"Usage: "<< argv[0]<<" <audio_file.wav>\n";
    return 1;
  }

  Audio audio(argv[1]);

  const auto& samples=audio.audioFile.decoded.samples;

  UI::initUI();
  bool running=true;
  std::string status="Press 'q' to quit.";

  int scrollIndex=0;
  int scrollSpeed=1;

  while(running){
    UI::renderUI(status,samples,scrollIndex);

    scrollIndex+=scrollSpeed;
    if(scrollIndex>=static_cast<int>(samples.size()))scrollIndex=0;// Loop

    int ch=getch();
    if(ch=='q'||ch=='Q')running=false;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  UI::shutdownUI();

  return 0;
}

int mmain(int argc,char** argv){
  using namespace SizzleFX;

  // Check for input .wav file
  if(argc<2){
    std::cerr<<"Usage: "<< argv[0]<<" <audio_file.wav>\n";
    return 1;
  }

  Audio audio;
  if(!audio.reload(argv[1]))return 1;

  const auto& samples=audio.audioFile.decoded.samples;

  UI::initUI();
  bool running=true;
  std::string status="Press 'q' to quit.";

  int scrollIndex=0;
  int scrollSpeed=1;

  while(running){
    UI::renderUI(status,samples,scrollIndex);

    scrollIndex+=scrollSpeed;
    if(scrollIndex>=static_cast<int>(samples.size()))scrollIndex=0;// Loop

    int ch=getch();
    if(ch=='q'||ch=='Q')running=false;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  UI::shutdownUI();
  return 0;
}
