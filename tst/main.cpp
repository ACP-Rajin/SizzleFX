#include <cstdio>
#include <ncurses.h>
#include "core/test.cpp"

int main(int argc,char** argv){
  if(argc<2){
    std::cerr<<"Usage: "<<argv[0]<<" <audio_file.wav>\n";
    return 1;
  }

  Audio audio(argv[1]);

  audio.am.print();

  // std::cout<<"Channels: "<<audio.channels<<"\n";
  // std::cout<<"Sample Rate: "<<audio.sampleRate<<"\n";
  // std::cout<<"Bit Depth: "<<audio.bitDepth<<"\n";
  // std::cout<<"Bit Rate: "<<audio.bitrate<<'\n';
  // std::cout<<"Duration: "<<audio.durationSeconds<<" sec\n";
  // std::cout<<"File Size: "<<audio.fileSizeBytes<<" bytes\n";
  // std::cout<<"Min Amplitude: "<<audio.minAmplitude<<'\n';
  // std::cout<<"Max Amplitude: "<<audio.maxAmplitude<<'\n';
  // std::cout<<"Average Amplitude: "<<audio.averageAmplitude<<'\n';
  // std::cout<<"RMS Amplitude: "<<audio.rmsAmplitude<<'\n';

  return 0;
}
