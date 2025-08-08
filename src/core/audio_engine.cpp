#include "audio_engine.hpp"
#include <sndfile.h>
#include <iostream>

namespace SizzleFX{
namespace Core{

  bool AudioEngine::loadWAV(const std::string& path){
    SF_INFO sfinfo;
    SNDFILE* file=sf_open(path.c_str(),SFM_READ,&sfinfo);

    if(!file){
      std::cerr<<"Failed to open audio file: "<<path<<std::endl;
      return false;
    }

    // Log some basic info
    std::cout<<"Loaded WAV file: "<<path<<"\n";
    std::cout<<"Channels: "<<sfinfo.channels<<"\n";
    std::cout<<"Sample rate: "<<sfinfo.samplerate<<"\n";
    std::cout<<"Frames: "<<sfinfo.frames<<"\n";

    std::vector<float>buffer(sfinfo.frames*sfinfo.channels);
    sf_readf_float(file,buffer.data(),sfinfo.frames);
    sf_close(file);

    // Keep only left channel (or mono)
    samples.clear();
    for(sf_count_t i=0;i<sfinfo.frames;++i)samples.push_back(buffer[i*sfinfo.channels]);//Left channel

    return true;
  }

  const std::vector<float>& AudioEngine::getSamples()const{return samples;}

}
}
