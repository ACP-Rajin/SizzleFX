#include "audio.hpp"

class Audio{
  public:
  std::string filePath;
  std::string format; // e.g.,"wav","mp3","ogg","obb"
  uint64_t fileSizeBytes;
  // Audio format info
  uint16_t audioFormatCode; // PCM=1, MP3=85, OGG=??? (depends on format)
  uint16_t numChannels;     // Mono=1,Stereo=2
  uint32_t sampleRate;
  uint32_t byteRate;        // bytes/sec (sampleRate * channels * bits/8)
  uint16_t bitsPerSample;   // 8,16,24,32 bits
  uint64_t totalSamples;    // total frames * channels
  uint64_t totalFrames;     // per-channel frames
  bool littleEndian;

  enum class SampleType{INT8,INT16,INT24,INT32,FLOAT32,FLOAT64};
  SampleType sampleType;

  double durationSeconds;
  double avgFrequency;

  float minAmplitude;
  float maxAmplitude;
  float rmsAmplitude;    // Root Mean Square amplitude
  bool clippingDetected; // True if > 0.99 amplitude found
  // Compression / bitrate info
  bool isVBR;           // Variable Bit Rate
  uint32_t bitrateKbps; // Average bitrate in kbps
  // Optional tags (ID3/Vorbis Comments)
  std::string title;
  std::string artist;
  std::string album;
  std::string year;
  // Audio data
  std::vector<float>samples;   // normalized audio samples [-1, 1]
  std::vector<uint8_t>rawData; // optional raw file data

  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  HeaderWAV header;// Optional for testing

  Audio(){}
  Audio(std::string path){reload(path);}
  Audio(std::vector<float>& samples,int channels,int sampleRate){reload(samples,channels,sampleRate);}

  void reload(std::string path){loadWAV(path);}
  void reload(std::vector<float>& samples,int channels,int sampleRate){
    this->samples=samples;
    this->numChannels=channels;
    this->sampleRate=sampleRate;
    totalFrames=samples.size()/channels;
    durationSeconds=static_cast<double>(totalFrames)/sampleRate;
  }

  void play(){
    Pa_Initialize();

    PaStream *stream;
    Pa_OpenDefaultStream(&stream,0,numChannels,paFloat32,sampleRate,paFramesPerBufferUnspecified,nullptr,nullptr);

    Pa_StartStream(stream);
    Pa_WriteStream(stream,samples.data(),totalFrames);
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

  inline size_t getSampleCount()const{return samples.size();}
  inline size_t getSamplesPerChannel()const{return numChannels?samples.size()/numChannels:0;}
  inline double getDuration()const{return sampleRate?static_cast<double>(totalFrames)/sampleRate:0.0;}

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
    this->filePath=filePath;
    format=header.format;
    file.seekg(0,std::ios::end);
    fileSizeBytes=file.tellg();
    file.seekg(sizeof(HeaderWAV),std::ios::beg);

    audioFormatCode=header.audioFormat;
    numChannels=header.numChannels;
    sampleRate=header.sampleRate;
    byteRate=header.byteRate;
    bitsPerSample=header.bitsPerSample;
    littleEndian=true; // WAV is little-endian

    // Figure out sample type
    switch(header.bitsPerSample){
      case 8:  this->sampleType=SampleType::INT8;break;
      case 16: this->sampleType=SampleType::INT16;break;
      case 24: this->sampleType=SampleType::INT24;break;
      case 32: this->sampleType=(header.audioFormat==3)?SampleType::FLOAT32:SampleType::INT32;break;
      default: std::cerr << "Unsupported bit depth: " << header.bitsPerSample << "\n";
               return false;
    }

    // Audio data reading
    size_t bytesPerSample=header.bitsPerSample/8;
    size_t totalSamples=header.subchunk2Size/bytesPerSample;
    this->totalSamples=totalSamples;
    totalFrames=totalSamples/header.numChannels;

    // Read raw data
    rawData.resize(header.subchunk2Size);
    file.read(reinterpret_cast<char*>(rawData.data()),header.subchunk2Size);

    if(!file){
      std::cerr << "Error: Could not read WAV data\n";
      return false;
    }

    // Convert to normalized float samples [-1, 1]
    samples.resize(totalSamples);
    const uint8_t* raw=rawData.data();

    if(header.bitsPerSample==8){
      for(size_t i=0;i<totalSamples;++i)samples[i]=(static_cast<int>(raw[i])-128)/128.0f;
    }else if(header.bitsPerSample==16){
      for(size_t i=0;i<totalSamples;++i){
        int16_t sample=*reinterpret_cast<const int16_t*>(raw + i * 2);
        samples[i]=sample / 32768.0f;
      }
    }else if(header.bitsPerSample==24){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample=(raw[i*3+2]<<24)|(raw[i*3+1]<<16)|(raw[i*3]<<8);
        sample >>= 8;
        samples[i]=sample / 8388608.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==1){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample= *reinterpret_cast<const int32_t*>(raw + i * 4);
        samples[i]=sample / 2147483648.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==3)for(size_t i=0;i<totalSamples;++i)samples[i]=*reinterpret_cast<const float*>(raw + i * 4);

    // Amplitude analysis
    minAmplitude=1.0f;
    maxAmplitude=-1.0f;
    double sumSquares=0.0;
    for(float s:samples){
      if(s<minAmplitude)minAmplitude=s;
      if(s>maxAmplitude)maxAmplitude=s;
      sumSquares += s * s;
    }
    rmsAmplitude=static_cast<float>(std::sqrt(sumSquares / samples.size()));
    clippingDetected=(maxAmplitude >= 0.99f || minAmplitude <= -0.99f);

    // Duration
    durationSeconds=static_cast<double>(totalFrames) / sampleRate;

    // No avgFrequency calculation yet (requires FFT)
    avgFrequency=0.0;

    // No compression info (WAV PCM is uncompressed)
    isVBR=false;
    bitrateKbps=static_cast<uint32_t>(byteRate * 8 / 1000);

    return true;
  }
};
