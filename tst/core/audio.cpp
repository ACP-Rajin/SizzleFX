#include "audio.hpp"
#include "audio.hpp"
#include <cmath>

class Audio{
  public:
  AudioFile audioFile;

  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  HeaderWAV header;// Optional for testing

  Audio(){}
  Audio(std::string path){reload(path);}
  Audio(std::vector<float>& samples,int channels,int sampleRate){reload(samples,channels,sampleRate);}

  void reload(std::string path){loadWAV(path);}
  void reload(std::vector<float>& samples,int channels,int sampleRate){
    audioFile.data.samples=samples;
    audioFile.formatInfo.numChannels=channels;
    audioFile.formatInfo.sampleRate=sampleRate;
    audioFile.formatInfo.totalFrames=samples.size()/channels;
    audioFile.playbackInfo.durationSeconds=static_cast<double>(audioFile.formatInfo.totalFrames)/sampleRate;
  }

  void play(){
    Pa_Initialize();

    PaStream *stream;
    Pa_OpenDefaultStream(&stream,0,audioFile.formatInfo.numChannels,paFloat32,audioFile.formatInfo.sampleRate,paFramesPerBufferUnspecified,nullptr,nullptr);

    Pa_StartStream(stream);
    Pa_WriteStream(stream,audioFile.data.samples.data(),audioFile.formatInfo.totalFrames);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    Pa_Terminate();
  }

  static void play(std::vector<float>& samples,int channels,int sampleRate){
    Pa_Initialize();

    PaStream *stream;
    Pa_OpenDefaultStream(&stream,0,channels,paFloat32,sampleRate,paFramesPerBufferUnspecified,nullptr,nullptr);

    size_t totalFrames=samples.size() / channels;

    Pa_StartStream(stream);
    Pa_WriteStream(stream,samples.data(),totalFrames);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    Pa_Terminate();
  }

  inline size_t getSampleCount()const{return audioFile.data.samples.size();}
  inline size_t getSamplesPerChannel()const{return audioFile.formatInfo.numChannels?audioFile.data.samples.size()/audioFile.formatInfo.numChannels:0;}
  inline double getDuration()const{return audioFile.formatInfo.sampleRate?static_cast<double>(audioFile.formatInfo.totalFrames)/audioFile.formatInfo.sampleRate:0.0;}

private:
  bool loadWAV(const std::string& filePath){
    std::ifstream file(filePath,std::ios::binary);

    if(!file){
      std::cerr << "Error: Could not open file " << filePath << "\n";
      return false;
    }

    file.read(reinterpret_cast<char*>(&header),sizeof(HeaderWAV));

    if(!file){
      std::cerr << "Error: Could not read WAV header\n";
      return false;
    }

    // Check header validity
    if(std::string(header.chunkID,4)!="RIFF"||std::string(header.format,4)!="WAVE"){
      std::cerr << "Error: Not a valid WAV file\n";
      return false;
    }

    // Fill this->basics
    this->audioFile.fileInfo.filePath=filePath;
    audioFile.fileInfo.format=header.format;
    file.seekg(0,std::ios::end);
    audioFile.fileInfo.fileSizeBytes=file.tellg();
    file.seekg(sizeof(HeaderWAV),std::ios::beg);

    audioFile.formatInfo.audioFormatCode=header.audioFormat;
    audioFile.formatInfo.numChannels=header.numChannels;
    audioFile.formatInfo.sampleRate=header.sampleRate;
    audioFile.formatInfo.byteRate=header.byteRate;
    audioFile.formatInfo.bitsPerSample=header.bitsPerSample;
    audioFile.formatInfo.littleEndian=true; // WAV is little-endian

    // Figure out sample type
    switch(header.bitsPerSample){
      case 8:  this->audioFile.formatInfo.sampleType=SampleType::INT8;break;
      case 16: this->audioFile.formatInfo.sampleType=SampleType::INT16;break;
      case 24: this->audioFile.formatInfo.sampleType=SampleType::INT24;break;
      case 32: this->audioFile.formatInfo.sampleType=(header.audioFormat==3)?SampleType::FLOAT32:SampleType::INT32;break;
      default: std::cerr << "Unsupported bit depth: " << header.bitsPerSample << "\n";
               return false;
    }

    // Audio data reading
    size_t bytesPerSample=header.bitsPerSample/8;
    size_t totalSamples=header.subchunk2Size/bytesPerSample;
    this->audioFile.formatInfo.totalSamples=totalSamples;
    audioFile.formatInfo.totalFrames=totalSamples/header.numChannels;

    // Read raw data
    audioFile.data.rawData.resize(header.subchunk2Size);
    file.read(reinterpret_cast<char*>(audioFile.data.rawData.data()),header.subchunk2Size);

    if(!file){
      std::cerr << "Error: Could not read WAV data\n";
      return false;
    }

    // Convert to normalized float samples [-1, 1]
    audioFile.data.samples.resize(totalSamples);
    const uint8_t* raw=audioFile.data.rawData.data();

    if(header.bitsPerSample==8){
      for(size_t i=0;i<totalSamples;++i)audioFile.data.samples[i]=(static_cast<int>(raw[i])-128)/128.0f;
    }else if(header.bitsPerSample==16){
      for(size_t i=0;i<totalSamples;++i){
        int16_t sample=*reinterpret_cast<const int16_t*>(raw + i * 2);
        audioFile.data.samples[i]=sample / 32768.0f;
      }
    }else if(header.bitsPerSample==24){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample=(raw[i*3+2]<<24)|(raw[i*3+1]<<16)|(raw[i*3]<<8);
        sample >>= 8;
        audioFile.data.samples[i]=sample / 8388608.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==1){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample= *reinterpret_cast<const int32_t*>(raw + i * 4);
        audioFile.data.samples[i]=sample / 2147483648.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==3)for(size_t i=0;i<totalSamples;++i)audioFile.data.samples[i]=*reinterpret_cast<const float*>(raw + i * 4);

    // Amplitude analysis
    audioFile.analysis.minAmplitude=1.0f;
    audioFile.analysis.maxAmplitude=-1.0f;
    double sumSquares=0.0;
    for(float s:audioFile.data.samples){
      if(s<audioFile.analysis.minAmplitude)audioFile.analysis.minAmplitude=s;
      if(s>audioFile.analysis.maxAmplitude)audioFile.analysis.maxAmplitude=s;
      sumSquares += s * s;
    }
    audioFile.analysis.rmsAmplitude=static_cast<float>(std::sqrt(sumSquares / audioFile.data.samples.size()));
    audioFile.analysis.clippingDetected=(audioFile.analysis.maxAmplitude >= 0.99f || audioFile.analysis.minAmplitude <= -0.99f);

    // Duration
    audioFile.playbackInfo.durationSeconds=static_cast<double>(audioFile.formatInfo.totalFrames) / audioFile.formatInfo.sampleRate;

    // No avgFrequency calculation yet (requires FFT)
    audioFile.playbackInfo.avgFrequency=0.0;

    // No compression info (WAV PCM is uncompressed)
    audioFile.compression.isVBR=false;
    audioFile.compression.bitrateKbps=static_cast<uint32_t>(audioFile.formatInfo.byteRate * 8 / 1000);

    return true;
  }
};
