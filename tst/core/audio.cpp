#include "audio.hpp"

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

  void reload(const std::string& path){
    std::string extention=path.substr(path.find_last_of('.')+1);
    if(extention=="wav"||extention=="wave"||extention=="WAV"||extention=="WAVE")
      loadWAV(path);
    else if(extention=="mp3"||extention=="MP3")
      loadMP3(path);
    else if(extention=="obb"||extention=="OBB")
      loadOBB(path);
    else if(extention=="opus"||extention=="OPUS")
      loadOPUS(path);
    else throw std::runtime_error("Unsupported audio format: "+extention);
  }
  void reload(std::vector<float>& samples,int channels,int sampleRate){
    audioFile.data.samples=samples;
    audioFile.formatInfo.numChannels=channels;
    audioFile.formatInfo.sampleRate=sampleRate;
    audioFile.formatInfo.totalFrames=samples.size()/channels;
    audioFile.formatInfo.totalSamples=samples.size();
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
  // --- Helper: little-endian integer reader ---
  template<typename T> T readLE(std::ifstream &in){
    T value{};
    in.read(reinterpret_cast<char*>(&value),sizeof(T));
    return value;
  }

  // --- Standalone WAV loader ---
  bool loadWAV(const std::string &path){
    audioFile={};//reset all fields

    audioFile.fileInfo.filePath=path;
    audioFile.fileInfo.format="wav";

    std::ifstream in(path,std::ios::binary | std::ios::ate);
    if(!in)return false;

    audioFile.fileInfo.fileSizeBytes=in.tellg();
    in.seekg(0,std::ios::beg);

    // ---- RIFF header ----
    char riff[4]; in.read(riff,4);
    if(std::string(riff,4) != "RIFF")return false;

    uint32_t fileSizeMinus8=readLE<uint32_t>(in);

    char wave[4]; in.read(wave,4);
    if(std::string(wave,4) != "WAVE")return false;

    // ---- Read chunks ----
    bool foundFmt=false,foundData=false;
    uint32_t dataChunkSize=0;
    std::vector<uint8_t>dataBuffer;

    while(in && (!foundFmt || !foundData)){
      char chunkId[4];
      if(!in.read(chunkId,4))break;
      uint32_t chunkSize=readLE<uint32_t>(in);

      std::string chunk(chunkId,4);

      if(chunk == "fmt "){
        audioFile.formatInfo.audioFormatCode=readLE<uint16_t>(in);
        audioFile.formatInfo.numChannels    =readLE<uint16_t>(in);
        audioFile.formatInfo.sampleRate     =readLE<uint32_t>(in);
        audioFile.formatInfo.byteRate       =readLE<uint32_t>(in);
        uint16_t blockAlign                 =readLE<uint16_t>(in);
        audioFile.formatInfo.bitsPerSample  =readLE<uint16_t>(in);

        // Skip extra params if present
        if(chunkSize>16)in.seekg(chunkSize-16,std::ios::cur);

        // Decide sample type
        switch(audioFile.formatInfo.bitsPerSample){
          case 8: audioFile.formatInfo.sampleType=SampleType::INT8;  break;
          case 16:audioFile.formatInfo.sampleType=SampleType::INT16; break;
          case 24:audioFile.formatInfo.sampleType=SampleType::INT24; break;
          case 32:audioFile.formatInfo.sampleType=SampleType::INT32; break;
          default:return false;//unsupported
        }

        foundFmt=true;
      }else if(chunk=="data"){
        dataChunkSize=chunkSize;
        dataBuffer.resize(chunkSize);
        in.read(reinterpret_cast<char*>(dataBuffer.data()),chunkSize);
        foundData=true;
      }else{
        // Skip irrelevant chunks
        in.seekg(chunkSize,std::ios::cur);
      }
    }

    if(!foundFmt || !foundData)return false;

    // ---- Fill format info ----
    uint64_t bytesPerSample=audioFile.formatInfo.bitsPerSample / 8;
    audioFile.formatInfo.totalFrames = dataChunkSize / (bytesPerSample * audioFile.formatInfo.numChannels);
    audioFile.formatInfo.totalSamples=audioFile.formatInfo.totalFrames * audioFile.formatInfo.numChannels;

    // ---- Playback info ----
    audioFile.playbackInfo.durationSeconds=static_cast<double>(audioFile.formatInfo.totalFrames) / audioFile.formatInfo.sampleRate;

    // ---- Store raw bytes ----
    audioFile.data.rawData=std::move(dataBuffer);

    // ---- Decode normalized samples ----
    audioFile.data.samples.reserve(audioFile.formatInfo.totalSamples);
    const uint8_t* p=audioFile.data.rawData.data();

    for(uint64_t i=0;i<audioFile.formatInfo.totalSamples;i++){
      float sample=0.0f;

      if(audioFile.formatInfo.bitsPerSample==8){
        int8_t v=static_cast<int8_t>(p[i] - 128);
        sample=static_cast<float>(v) / 128.0f;
      }else if(audioFile.formatInfo.bitsPerSample==16){
        int16_t v= *reinterpret_cast<const int16_t*>(p + i*2);
        sample=static_cast<float>(v) / 32768.0f;
      }else if(audioFile.formatInfo.bitsPerSample==24){
        int32_t v=(p[i*3] | (p[i*3+1] << 8) | (p[i*3+2] << 16));
        if(v & 0x800000)v |= ~0xFFFFFF; // sign extend
        sample=static_cast<float>(v) / 8388608.0f;
      }else if(audioFile.formatInfo.bitsPerSample==32){
        int32_t v= *reinterpret_cast<const int32_t*>(p + i*4);
        sample=static_cast<float>(v) / 2147483648.0f;
      }

      audioFile.data.samples.push_back(sample);
    }

    // ---- Simple analysis ----
    if(!audioFile.data.samples.empty()){
      audioFile.analysis.minAmplitude= *std::min_element(audioFile.data.samples.begin(),audioFile.data.samples.end());
      audioFile.analysis.maxAmplitude= *std::max_element(audioFile.data.samples.begin(),audioFile.data.samples.end());

      double sumSq=0.0;
      for(float s : audioFile.data.samples)sumSq += s*s;
      audioFile.analysis.rmsAmplitude=std::sqrt(sumSq / audioFile.data.samples.size());

      audioFile.analysis.clippingDetected=(audioFile.analysis.maxAmplitude >= 0.999f || audioFile.analysis.minAmplitude <= -0.999f);
    }

    return true;
  }
  bool loadMP3(const std::string& path){
    std::cout << "I am MP3\n";
    return true;
  }
  bool loadOBB(const std::string& path){
    std::cout << "I am OBB\n";
    return true;
  }
  bool loadOPUS(const std::string& path){
    std::cout << "I am OPUS\n";
    return true;
  }
};
