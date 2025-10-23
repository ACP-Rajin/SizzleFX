#include "audio.hpp"

class Audio{
  public:
  AudioFile audioFile;

  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  enum PlaybackState:int{
    Stopped=0,
    Playing=1,
    Paused=2
  };

  private:
  // PortAudio stream
  PaStream *stream=nullptr;

  // Atomic shared state for callback thread safety
  std::atomic<uint64_t>currentFrame{0}; // frames (not samples)
  std::atomic<PlaybackState>state{PlaybackState::Stopped};
  std::atomic<bool>loopEnabled{false};
  std::atomic<uint32_t>loopCount{0};    // 0 => infinite, >0 => that many plays
  std::atomic<uint32_t>playedLoops{0};  // how many full plays completed

  // Static ref count for Pa_Initialize / Pa_Terminate
  static std::atomic<int> paInstanceCount;
  static std::once_flag paInitFlag;
  static std::mutex streamOpenMutex;

  public:
  Audio(){incrementPaRef();}
  Audio(const std::string& path){reload(path);}
  Audio(const std::vector<float>& samples,int channels,int sampleRate){reload(samples,channels,sampleRate);}

  ~Audio(){
    stop(); // ensure stream stopped & closed
    closeStreamIfOpen();
    decrementPaRef();
  }

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

    // reset analysis & tags/codec/fileinfo left intact by caller if desired
    audioFile.analysis=Analysis{};
    audioFile.codecInfo=CodecInfo{};
    // reset playback control state
    currentFrame.store(0);
    playedLoops.store(0);
    loopCount.store(0);
    loopEnabled.store(false);
    state.store(PlaybackState::Stopped);
  }

  bool play(){
    if(!hasDecodedData())return false;

    PlaybackState expected=PlaybackState::Stopped;
    // If we were stopped, set currentFrame to currentFrame (could be non-zero if user seeks while stopped).
    // If we were paused, calling play should behave like resume()
    if(state.load()==PlaybackState::Playing)return true;

    if(!openStreamIfNeeded())return false;

    state.store(PlaybackState::Playing);
    PaError err=Pa_StartStream(stream);
    if(err!=paNoError){
      std::cerr << "Pa_StartStream failed: " << Pa_GetErrorText(err) << "\n";
      return false;
    }
    return true;
  }
  
  bool play(std::vector<float>& samples,int channels,int sampleRate){
    reload(samples,channels,sampleRate);
    play();
  }

  static bool playOneShot(std::vector<float> &samples,int channels,int sampleRate){
    Audio tmp(samples,channels,sampleRate);
    return tmp.play();
  }

  void pause(){if(state.load()==PlaybackState::Playing)state.store(PlaybackState::Paused);}
  void resume(){
    if(state.load()==PlaybackState::Paused){
      state.store(PlaybackState::Playing);
    }else if(state.load()==PlaybackState::Stopped){
      play();
    }
  }
  void stop(){
    if(stream){
      // request PortAudio to stop
      PaError err=Pa_StopStream(stream);
      if (err!=paNoError && err!=paStreamIsStopped)std::cerr << "Pa_StopStream failed: " << Pa_GetErrorText(err) << "\n";
    }
    state.store(PlaybackState::Stopped);
    currentFrame.store(0);
    playedLoops.store(0);
  }

  void setIsLoop(bool enable){
    loopEnabled.store(enable);
    if(!enable)loopCount.store(0);
  }
  void setLoopCount(uint32_t n){
    loopEnabled.store(true);
    loopCount.store(n); // 0 => infinite, >0 => number of times to play
    playedLoops.store(0);
  }
  void setPositionInSeconds(double seconds){
    if(!hasDecodedData())return;
    uint64_t sr=audioFile.playbackInfo.sampleRate;
    if(sr==0) return;
    uint64_t target=static_cast<uint64_t>(seconds * sr);
    uint64_t maxFrames=audioFile.decoded.totalFrames;
    if(target>=maxFrames)target=maxFrames?maxFrames-1:0;
    currentFrame.store(target);
  }

  inline size_t getSampleCount()const{return audioFile.decoded.samples.size();}
  inline size_t getSamplesPerChannel()const{return audioFile.playbackInfo.numChannels?audioFile.decoded.samples.size()/audioFile.playbackInfo.numChannels:0;}
  inline double getDuration()const{return audioFile.playbackInfo.sampleRate?static_cast<double>(audioFile.decoded.totalFrames)/audioFile.playbackInfo.sampleRate:0.0;}
  inline bool getIsLoop()const{return loopEnabled;}
  inline uint32_t getLoopCount()const{return loopCount;}
  inline double getPositionInSeconds()const{
    uint64_t frame=currentFrame.load();
    if (audioFile.playbackInfo.sampleRate==0)return 0.0;
    return static_cast<double>(frame) / static_cast<double>(audioFile.playbackInfo.sampleRate);
  }

  private:
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

  // ---------------- PortAudio callback ----------------
  static int paCallback(const void *inputBuffer,void *outputBuffer,unsigned long framesPerBuffer,const PaStreamCallbackTimeInfo *timeInfo,PaStreamCallbackFlags statusFlags,void *userData){
    Audio *self=reinterpret_cast<Audio*>(userData);
    float *out=reinterpret_cast<float*>(outputBuffer);
    if (!self || !out) return paContinue;

    const uint16_t channels=static_cast<uint16_t>(self->audioFile.playbackInfo.numChannels);
    const uint64_t totalFrames=self->audioFile.decoded.totalFrames;
    const float *source=self->audioFile.decoded.samples.data();

    // Handle no data
    if(totalFrames==0 || channels==0 || !source){
      std::fill(out,out + framesPerBuffer * channels,0.0f);
      return paContinue;
    }

    // If paused: output silence
    if(self->state.load()==PlaybackState::Paused){
      std::fill(out,out + framesPerBuffer * channels,0.0f);
      return paContinue;
    }

    uint64_t framePos=self->currentFrame.load();
    for(unsigned long f=0;f<framesPerBuffer;++f){
      if(framePos>=totalFrames){
        bool loop=self->loopEnabled.load();
        uint32_t lc=self->loopCount.load();

        if(loop && (lc==0 || self->playedLoops.load() < lc)){
          self->playedLoops.fetch_add(1);
          framePos=0;
          self->currentFrame.store(0);
        }else{
          self->state.store(PlaybackState::Stopped);
          std::fill(out + f * channels,out + framesPerBuffer * channels,0.0f);
          self->currentFrame.store(framePos);
          return paContinue; // <--- never complete, stays alive
        }
      }

      size_t srcIndex=static_cast<size_t>(framePos) * channels;
      for(uint16_t c=0;c<channels;++c)out[f * channels + c]=source[srcIndex+c];
      ++framePos;
    }

    self->currentFrame.store(framePos);
    return paContinue;
  }

  // ---------------- Stream management ----------------
  bool openStreamIfNeeded(){
    std::lock_guard<std::mutex>lock(streamOpenMutex);
    if(stream){
      // stream already open
      return true;
    }

    if(!hasDecodedData())return false;

    // Ensure PortAudio initialized
    std::call_once(paInitFlag,[](){Pa_Initialize();});

    PaError err=Pa_OpenDefaultStream(
      &stream,
      0, // no input
      static_cast<int>(audioFile.playbackInfo.numChannels),
      paFloat32,
      static_cast<double>(audioFile.playbackInfo.sampleRate),
      paFramesPerBufferUnspecified,
      &Audio::paCallback,
      this
    );

    if(err!=paNoError){
      std::cerr << "Pa_OpenDefaultStream failed: " << Pa_GetErrorText(err) << "\n";
      stream=nullptr;
      return false;
    }

    return true;
  }

  void closeStreamIfOpen(){
    std::lock_guard<std::mutex> lock(streamOpenMutex);
    if(!stream)return;

    PaError err;
    if(Pa_IsStreamActive(stream)==1){
      err=Pa_StopStream(stream);
      if(err!=paNoError && err!=paStreamIsStopped)std::cerr << "Pa_StopStream failed while closing: " << Pa_GetErrorText(err) << "\n";
    }

    err=Pa_CloseStream(stream);
    if(err!=paNoError)std::cerr << "Pa_CloseStream failed: " << Pa_GetErrorText(err) << "\n";
    stream = nullptr;
  }

  static void incrementPaRef(){
    int prev=paInstanceCount.fetch_add(1);
    if(prev==0){
      // first instance: initialize PortAudio
      std::call_once(paInitFlag,[](){Pa_Initialize();});
    }
  }

  static void decrementPaRef(){
    int prev=paInstanceCount.fetch_sub(1);
    if(prev==1){
      // last instance destroyed -> terminate PortAudio
      Pa_Terminate();
    }
  }

  bool hasDecodedData()const{return(audioFile.decoded.totalFrames>0 && audioFile.playbackInfo.numChannels>0 && !audioFile.decoded.samples.empty());}
};
// --- static member definitions (put in the header after the class or in a single cpp) ---
std::atomic<int> Audio::paInstanceCount{0};
std::once_flag Audio::paInitFlag;
std::mutex Audio::streamOpenMutex;
