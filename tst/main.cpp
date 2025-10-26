#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <sstream>
#include "core/audio.cpp"
/* // Test 1
void printHeader(HeaderWAV &header){
  std::cout << "WAV Header:\n Chunk ID: ";
  std::cout.write(header.chunkID,4); std::cout << "\n";
  std::cout << "  Chunk Size: " << header.chunkSize << "\n Format: ";
  std::cout.write(header.format,4); std::cout << "\n Subchunk1 ID: ";
  std::cout.write(header.subchunk1ID,4); std::cout << "\n";
  std::cout << "  Subchunk1 Size: " << header.subchunk1Size << "\n";
  std::cout << "  Audio Format: " << header.audioFormat << "\n";
  std::cout << "  Channels: " << header.numChannels << "\n";
  std::cout << "  Sample Rate: " << header.sampleRate << "\n";
  std::cout << "  Byte Rate: " << header.byteRate << "\n";
  std::cout << "  Block Align: " << header.blockAlign << "\n";
  std::cout << "  Bits Per Sample: " << header.bitsPerSample << "\n Subchunk2 ID: ";
  std::cout.write(header.subchunk2ID,4); std::cout << "\n";
  std::cout << "  Subchunk2 Size: " << header.subchunk2Size << "\n";
}
void printMetadata(Audio &metadata){
  std::cout << "Basic Info:\n";
  std::cout << "  File Path: " << metadata.filePath << '\n';
  std::cout << "  Format: " << metadata.format << '\n';
  std::cout << "  File Size: " << metadata.fileSizeBytes << " bytes\n\n";

  std::cout << "Audio Format Info:\n";
  std::cout << "  Audio Format Code: " << metadata.audioFormatCode << '\n';
  std::cout << "  Channels: " << metadata.numChannels << '\n';
  std::cout << "  Sample Rate: " << metadata.sampleRate << '\n';
  std::cout << "  Byte Rate: " << metadata.byteRate << '\n';
  std::cout << "  Bits Per Sample: " << metadata.bitsPerSample << '\n';
  std::cout << "  Total Samples:" << metadata.totalSamples << '\n';
  std::cout << "  Total Frames: " << metadata.totalFrames << '\n';
  std::cout << "  Little-Endian?: " << metadata.littleEndian << "\n\n";

  std::cout << "Time & Frequency:\n";
  std::cout << "  Duration: " << metadata.durationSeconds << " sec\n";
  std::cout << "  Average Frequency: " << metadata.avgFrequency << " Hz\n";

  std::cout << "Amplitude Analysis:\n";
  std::cout << "  Max Amplitude: " << metadata.maxAmplitude << '\n';
  std::cout << "  RMS Amplitude: " << metadata.rmsAmplitude << '\n';
  std::cout << "  Clipping Detected?: " << metadata.clippingDetected << "\n\n";

  std::cout << "Compression / byteRate Info:\n";
  std::cout << "  Is Variable Bit Rate (VBR)?: " << metadata.isVBR << '\n';
  std::cout << "  Bit Rate: " << metadata.bitrateKbps << " kbps\n\n";

  std::cout << "Optional Tags:\n";
  std::cout << "  Title: " << metadata.title << '\n';
  std::cout << "  Artist: " << metadata.artist << '\n';
  std::cout << "  Album: " << metadata.album << '\n';
  std::cout << "  Year: " << metadata.year << "\n\n";

  std::cout << "Others:\n";
  std::cout << "  Sample Count: " << metadata.getSampleCount() << '\n';
  std::cout << "  Samples Per Channel: " << metadata.getSamplesPerChannel() << '\n';
  std::cout << "  Duration: " << metadata.getDuration() << '\n';
}
*/
/*
std::vector<std::string>split(const std::string& str,char delimiter){
  std::vector<std::string>tokens;
  std::stringstream ss(str);
  std::string word;
  while(std::getline(ss,word,delimiter))tokens.push_back(word);
  return tokens;
} 

void stdInOut(std::string prompt,std::string& var){
  std::cout << prompt;
  std::getline(std::cin,var);
}

int main(){
  // if(argc<2){
  //   std::cerr<<"Usage: "<<argv[0]<<" <audio_file.wav>\n";
  //   return 1;
  // }

  Audio audio("samples/output.wav");

  std::string command;
  std::string tmp,tmpSamples,tmpChannels,tmpSampleRate;
  while(true){
    stdInOut("\nEnter Command: ",command);
    if(command=="exit"){
      std::cout << "Exiting..." << std::endl;
      break; 
    }

    std::vector<std::string>word=split(command,' ');
    if(!word.empty()){
      if(word[0]=="clear")system("clear");
      if(word[0]=="term"){
        if(word.size()>1)for(size_t i=1;i<word.size();i++)tmp+=word[i]+' ';
        system(tmp.c_str());
        tmp="";
      }

      if(word[0]=="load")audio.reload(word[1]);
      if(word[0]=="play"){
        audio.play();
        std::vector<float>samples=audio.audioFile.decoded.samples;
        std::cout << "Samples: " << samples.size() << "\nSample Rate: " << audio.audioFile.playbackInfo.sampleRate << "\n";
      }
      // if(word[0]=="header")printHeader(audio.header);
      // if(word[0]=="metadata")printMetadata(audio);
    }
  }

  return 0;
}
*/
// Test 2
int main(int argc,char *argv[]){
  // if(argc<2)return 1;

  Audio audio("samples/o.wav"),audio1("samples/game_over.wav");
  if(argc>1)audio.reload(static_cast<std::string>(argv[1]));

  setlocale(LC_ALL,"");
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);
  timeout(100);

  bool running=true;
  std::string status="Press 'q' to quit.";

  while(running){
    clear();
    mvprintw(0,0,"SizzleFX Audio Editor — Press Q to Quit");
    mvprintw(1,0,"[S] Play [P] Pause [R] Resume [D] Stop [E] Seek [L] Loop [←][→] Seek ±0.5s");

    mvprintw(3,0,"Status: %s",audio.getState()==Audio::PlaybackState::Playing?(audio.getIsLoop()?"Playing (Looping)":"Playing"):(audio.getIsLoop()?"Loop Ready":"Stopped"));

    mvprintw(4,0,"Position: %.2lf / %.2f sec",audio.getPositionInSeconds(),audio.getDuration());

    refresh();

    int ch=getch();
    switch(ch){
      case 'q': case 'Q': running=false;break;
      case 's': case 'S': audio.play();break;
      case 'p': case 'P': audio.pause();break;
      case 'r': case 'R': audio.resume();break;
      case 'd': case 'D': audio.stop();break;
      case 'e': case 'E': audio.setPositionInSeconds(2.0);break;
      case 'l': case 'L': audio.setIsLoop(!audio.getIsLoop());break;
      case KEY_LEFT:      audio.setPositionInSeconds(audio.getPositionInSeconds()-0.5);break;
      case KEY_RIGHT:     audio.setPositionInSeconds(audio.getPositionInSeconds()+0.5);break;
    }
  }

  delwin(stdscr);
  endwin();
  return 0;
}
