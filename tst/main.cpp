#include <iostream>
#include <ncurses.h>
#include "core/test.cpp"

int main(int argc,char** argv){
  // if(argc<2){
  //   std::cerr<<"Usage: "<< argv[0]<<" <audio_file.wav>\n";
  //   return 1;
  // }
  //
  // if(!engine.loadWAV(argv[1]))return 1;

  Audio audio("output.wav");

  if (audio.isValid) {
    std::cout << "Channels: " << audio.channels << "\n";
    std::cout << "Sample Rate: " << audio.sampleRate << "\n";
    std::cout << "Bit Depth: " << audio.bitDepth << "\n";
    std::cout << "Duration: " << audio.durationSeconds << " sec\n";
    std::cout << "File Size: " << audio.fileSizeBytes << " bytes\n";
  }

  return 0;
}
