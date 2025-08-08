#include "ui/screen.hpp"
#include "core/audio_engine.hpp"

#include <chrono>
#include <thread>
#include <ncurses.h>
#include <iostream>

int main(int argc,char** argv){
  using namespace SizzleFX;

  // Check for input .wav file
  if(argc<2){
    std::cerr<<"Usage: "<< argv[0]<<" <audio_file.wav>\n";
    return 1;
  }

  Core::AudioEngine engine;
  if(!engine.loadWAV(argv[1]))return 1;

  const auto& samples=engine.getSamples();

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
