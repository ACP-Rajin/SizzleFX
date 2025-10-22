#include "audio.hpp"

class Audio{
  public:
  AudioFile audioFile;

  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  Audio(){}
  Audio(std::string path){reload(path);}
  Audio(const std::vector<float>& samples,int channels,int sampleRate){reload(samples,channels,sampleRate);}

  bool reload(const std::string& path){
    std::string extention=path.substr(path.find_last_of('.')+1);
    if(extention=="wav"||extention=="wave"||extention=="WAV"||extention=="WAVE")return loadWAV(path);
    else if(extention=="mp3"||extention=="MP3")return loadMP3(path);
    else if(extention=="obb"||extention=="OBB")return loadOBB(path);
    else if(extention=="ogg"||extention=="OGG")return loadOGG(path);
    else if(extention=="opus"||extention=="OPUS")return loadOPUS(path);
    else throw std::runtime_error("Unsupported audio format: "+extention);
  }
  void reload(const std::vector<float>& samples,int channels,int sampleRate){
    audioFile.decoded.samples=samples;
    audioFile.playbackInfo.numChannels=channels;
    audioFile.playbackInfo.sampleRate=sampleRate;
    audioFile.decoded.totalFrames=samples.size()/channels;
    audioFile.playbackInfo.durationSeconds=static_cast<double>(audioFile.decoded.totalFrames)/sampleRate;
  }

  void play(){
    Pa_Initialize();

    PaStream *stream;
    Pa_OpenDefaultStream(&stream,0,audioFile.playbackInfo.numChannels,paFloat32,audioFile.playbackInfo.sampleRate,paFramesPerBufferUnspecified,nullptr,nullptr);

    Pa_StartStream(stream);
    Pa_WriteStream(stream,audioFile.decoded.samples.data(),audioFile.decoded.totalFrames);
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

  inline size_t getSampleCount()const{return audioFile.decoded.samples.size();}
  inline size_t getSamplesPerChannel()const{return audioFile.playbackInfo.numChannels?audioFile.decoded.samples.size()/audioFile.playbackInfo.numChannels:0;}
  inline double getDuration()const{return audioFile.playbackInfo.sampleRate?static_cast<double>(audioFile.decoded.totalFrames)/audioFile.playbackInfo.sampleRate:0.0;}

private:
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

    uint16_t numChannels=0;
    uint32_t sampleRate=0;
    uint16_t bitsPerSample=0;
    uint16_t blockAlign=0;
    uint32_t byteRate=0;

    while(in && (!foundFmt || !foundData)){
      char chunkId[4];
      if(!in.read(chunkId,4))break;
      uint32_t chunkSize=readLE<uint32_t>(in);

      std::string chunk(chunkId,4);

      if(chunk == "fmt "){
        uint16_t audioFormatCode=readLE<uint16_t>(in);
        numChannels             =readLE<uint16_t>(in);
        sampleRate              =readLE<uint32_t>(in);
        byteRate                =readLE<uint32_t>(in);
        blockAlign              =readLE<uint16_t>(in);
        bitsPerSample           =readLE<uint16_t>(in);

        // Skip extra params if present
        if(chunkSize>16)in.seekg(chunkSize-16,std::ios::cur);

        // Decide sample type
        switch(bitsPerSample){
          case 8: break;
          case 16:break;
          case 24:break;
          case 32:break;
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
    uint64_t bytesPerSample=bitsPerSample / 8;
    uint64_t totalFrames=dataChunkSize / (bytesPerSample * numChannels);
    // audioFile.formatInfo.totalSamples=audioFile.formatInfo.totalFrames * audioFile.formatInfo.numChannels;

    // ---- Fill playback info ----
    audioFile.playbackInfo.sampleRate=sampleRate;
    audioFile.playbackInfo.numChannels=numChannels;
    audioFile.playbackInfo.durationSeconds=static_cast<double>(totalFrames) / sampleRate;

    // ---- Fill codec info ----
    audioFile.codecInfo.codecName="PCM";
    audioFile.codecInfo.bitrateKbps=(byteRate * 8) / 1000;
    audioFile.codecInfo.isVBR=false;
    audioFile.codecInfo.extra["bitsPerSample"]=std::to_string(bitsPerSample);
    audioFile.codecInfo.extra["blockAlign"]=std::to_string(blockAlign);
    audioFile.codecInfo.extra["byteRate"]=std::to_string(byteRate);

    // ---- Store decoded PCM ----
    audioFile.decoded.totalFrames=totalFrames;
    audioFile.decoded.samples.reserve(totalFrames * numChannels);

    const uint8_t* p=dataBuffer.data();

    for(uint64_t i=0;i<totalFrames * numChannels;i++){
      float sample=0.0f;

      if(bitsPerSample==8){
        int8_t v=static_cast<int8_t>(p[i] - 128);
        sample=static_cast<float>(v) / 128.0f;
      }else if(bitsPerSample==16){
        int16_t v= *reinterpret_cast<const int16_t*>(p + i * 2);
        sample=static_cast<float>(v) / 32768.0f;
      }else if(bitsPerSample==24){
        int32_t v=(p[i * 3] | (p[i * 3 + 1] << 8) | (p[i * 3 + 2] << 16));
        if(v & 0x800000)v |= ~0xFFFFFF; // sign extend
        sample=static_cast<float>(v) / 8388608.0f;
      }else if(bitsPerSample==32){
        int32_t v= *reinterpret_cast<const int32_t*>(p + i * 4);
        sample=static_cast<float>(v) / 2147483648.0f;
      }

      audioFile.decoded.samples.push_back(sample);
    }

    // ---- Simple analysis ----
    if(!audioFile.decoded.samples.empty()){
      audioFile.analysis.minAmplitude= *std::min_element(audioFile.decoded.samples.begin(),audioFile.decoded.samples.end());
      audioFile.analysis.maxAmplitude= *std::max_element(audioFile.decoded.samples.begin(),audioFile.decoded.samples.end());

      double sumSq=0.0;
      for(float s : audioFile.decoded.samples)sumSq += s*s;
      audioFile.analysis.rmsAmplitude=std::sqrt(sumSq / audioFile.decoded.samples.size());

      audioFile.analysis.clippingDetected=(audioFile.analysis.maxAmplitude >= 0.999f || audioFile.analysis.minAmplitude <= -0.999f);
    }

    return true;
  }

  bool loadMP3(const std::string& path){
    std::cout << "I am MP3 at" << path << "\n";
    return true;
  }
  bool loadOBB(const std::string& path){
    std::cout << "I am OBB at" << path << "\n";
    return true;
  }
  bool loadOGG(const std::string& path){
    std::cout << "I am OGG at" << path << "\n";
    return true;
  }
  bool loadOPUS(const std::string& path){
    std::cout << "I am OPUS at" << path << "\n";
    return true;
  }
};

class AudioPlayer{
  std::vector<float>samples;
  int numChannels=0;
  int sampleRate=0;
  uint64_t totalFrames=0;
  std::atomic<uint64_t>currentFrame{0};

  PaStream* stream=nullptr;

  std::atomic<bool>isPlaying{false};
  std::atomic<bool>isPaused{false};

  // Loop state
  std::atomic<bool>loopEnabled{false};
  std::atomic<uint64_t>loopStartFrame{0};
  std::atomic<uint64_t>loopEndFrame{0};

  public:
  AudioPlayer()=default;
  ~AudioPlayer(){stop();}

  bool load(const std::vector<float>& samples,int channels,int sampleRate){
    if(samples.empty()||channels<=0||sampleRate<=0)return false;

    this->samples=samples;
    this->numChannels=channels;
    this->sampleRate=sampleRate;
    totalFrames=samples.size()/channels;
    currentFrame.store(0);

    loopEnabled=false;
    loopStartFrame=0;
    loopEndFrame=totalFrames;

    return true;
  }

  bool start(){
    if(samples.empty())return false;
    if(isPlaying)return true; // already playing

    Pa_Initialize();

    PaStreamParameters outputParams{};
    outputParams.device=Pa_GetDefaultOutputDevice();
    outputParams.channelCount=numChannels;
    outputParams.sampleFormat=paFloat32;
    outputParams.suggestedLatency=Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;

    Pa_OpenStream(&stream,nullptr,&outputParams,sampleRate,paFramesPerBufferUnspecified,paClipOff,&AudioPlayer::paCallback,this);

    isPlaying=true;
    isPaused=false;

    Pa_StartStream(stream);
    return true;
  }

  void pause(){isPaused=true;}
  void resume(){isPaused=false;}

  void stop(){
    if(!isPlaying)return;
    isPlaying=false;

    if(stream){
      Pa_StopStream(stream);
      Pa_CloseStream(stream);
      stream=nullptr;
    }

    Pa_Terminate();
  }

  void seek(double seconds){
    if(sampleRate<=0)return;
    uint64_t target=static_cast<uint64_t>(seconds * sampleRate);
    if(target>=totalFrames)target=totalFrames-1;
    currentFrame.store(target);
  }

  // ----- Looping Controls -----
  void setLoop(bool enabled){loopEnabled=enabled;}

  void setLoopRegion(double startSec,double endSec){
    if(sampleRate<=0)return;
    uint64_t start=static_cast<uint64_t>(startSec * sampleRate),end=static_cast<uint64_t>(endSec * sampleRate);

    if(end>totalFrames)end=totalFrames;
    if(start>=end)start=0; // safety
    loopStartFrame=start;
    loopEndFrame=end;
  }

  void resetLoopRegion(){
    loopStartFrame=0;
    loopEndFrame=totalFrames;
  }

  // ----- Info -----
  bool isLooping()const{return loopEnabled;}
  bool isPlayingNow()const{return isPlaying && !isPaused;}

  double getLoopStartSec()const{return static_cast<double>(loopStartFrame)/sampleRate;}
  double getLoopEndSec()const{return static_cast<double>(loopEndFrame)/sampleRate;}

  double getDurationSeconds()const{return sampleRate?static_cast<double>(totalFrames)/sampleRate:0.0;}
  double getCurrentTime()const{return sampleRate?static_cast<double>(currentFrame.load())/sampleRate:0.0;}

  private:
  // static int paCallback(const void* input, void* output,
  //     unsigned long frameCount,
  //     const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
  //     void* userData) {
  //   auto* self = reinterpret_cast<AudioPlayer*>(userData);
  //   float* out = reinterpret_cast<float*>(output);
  //
  //   if (!self->isPlaying) return paComplete;
  //
  //   if (self->isPaused) {
  //     std::fill(out, out + frameCount * self->numChannels, 0.0f);
  //     return paContinue;
  //   }
  //
  //   uint64_t start = self->currentFrame.load();
  //   uint64_t end = start + frameCount;
  //
  //   if (end > self->totalFrames) {
  //     end = self->totalFrames;
  //     self->isPlaying = false;
  //   }
  //
  //   uint64_t samplesToCopy = (end - start) * self->numChannels;
  //   std::copy_n(self->samples.data() + start * self->numChannels, samplesToCopy, out);
  //
  //   if (end < self->totalFrames)
  //     std::fill(out + samplesToCopy, out + frameCount * self->numChannels, 0.0f);
  //
  //   self->currentFrame.store(end);
  //   return self->isPlaying ? paContinue : paComplete;
  // }

  static int paCallback(const void* input,void* output,unsigned long frameCount,const PaStreamCallbackTimeInfo*,PaStreamCallbackFlags,void* userData){
    auto* self=reinterpret_cast<AudioPlayer*>(userData);
    float* out=reinterpret_cast<float*>(output);

    if(!self->isPlaying)return paComplete;

    if(self->isPaused){
      std::fill(out,out+frameCount * self->numChannels,0.0f);
      return paContinue;
    }

    uint64_t start=self->currentFrame.load();
    uint64_t end=start+frameCount;

    for(unsigned long i=0;i<frameCount;++i){
      if(start>=self->loopEndFrame){
        if(self->loopEnabled){
          start=self->loopStartFrame; // wrap around
        }else{
          self->isPlaying=false;
          // fill remaining buffer with silence
          std::fill(out+i * self->numChannels,out+frameCount * self->numChannels,0.0f);
          self->currentFrame.store(self->totalFrames);
          return paComplete;
        }
      }

      for(int ch=0;ch<self->numChannels;++ch)out[i * self->numChannels + ch]=self->samples[start * self->numChannels + ch];
      ++start;
    }

    self->currentFrame.store(start);
    return paContinue;
  }
};
