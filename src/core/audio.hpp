#pragma once

#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <portaudio.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include <sndfile.hh>

// File-level metadata
struct FileInfo{
  std::string filePath;
  std::string format;       // "wav", "mp3", "ogg", "opus", ...
  uint64_t fileSizeBytes{};
};

// Playback-level info (unified)
struct PlaybackInfo{
  double durationSeconds{};   // total play length totalSamples / sampleRate
  uint32_t sampleRate{};      // hz (decoded or nominal)
  uint16_t numChannels{};     // 1=mono, 2=stereo, etc.
};

// Codec/container-level info
struct CodecInfo{
  std::string codecName;      // "PCM", "MP3", "Vorbis", "Opus", etc.
  uint32_t bitrateKbps{};     // average or nominal
  bool isVBR{false};          // true if variable bitrate
  std::map<std::string,std::string>extra; // flexible metadata (frame size, profile, etc.)
};

// Optional decoded audio (if loaded/decoded to PCM)
struct DecodedAudio{
  std::vector<float>samples; // normalized [-1,1], interleaved
  uint64_t totalFrames{};    // samples per channel
};

// Signal analysis
struct Analysis{
  float minAmplitude{};      
  float maxAmplitude{};
  float rmsAmplitude{};      // Root Mean Square loudness
  bool clippingDetected{false};
};

// Metadata tags (ID3, Vorbis, OpusTags, etc.)
struct Tags{
  std::string title;
  std::string artist;
  std::string album;
  std::string year;
  std::map<std::string,std::string>extra;// e.g. "genre", "comment"
};

// Unified Audio Metadata struct
struct AudioFile{
  FileInfo fileInfo;
  PlaybackInfo playbackInfo;
  CodecInfo codecInfo;
  DecodedAudio decoded;
  Analysis analysis;
  Tags tags;
};

// ---------------------------- Utils ----------------------------
// --- Helper: little-endian integer reader ---
template<typename T> T readLE(std::ifstream &in){
  T value{};
  in.read(reinterpret_cast<char*>(&value),sizeof(T));
  return value;
}
// Helper to safely get string tags
std::string getStringTag(SNDFILE* sndfile,int str_type){
  const char* str=sf_get_string(sndfile,str_type);
  return str ? std::string(str) : "";
}

/*
#pragma pack(push, 1)
struct HeaderWAV{
  ...
};
#pragma pack(pop)
*/

/*
 * Class definition
*/
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

  bool reload(const std::string& path){return loadAudioFile(path);}
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
    if(state.load()==PlaybackState::Stopped)setPositionInSeconds(0);
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

  static bool playOneShot(const std::string& path){
    Audio tmp(path);
    return tmp.play();
  }

  static bool playOneShot(std::vector<float> &samples,int channels,int sampleRate){
    Audio tmp(samples,channels,sampleRate);
    return tmp.play();
  }

  void pause(){if(state.load()==PlaybackState::Playing)state.store(PlaybackState::Paused);}
  void resume(){if(state.load()==PlaybackState::Paused)state.store(PlaybackState::Playing);}
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
  inline PlaybackState getState()const{return state;}
  inline bool getIsLoop()const{return loopEnabled;}
  inline uint32_t getLoopCount()const{return loopCount;}
  inline double getPositionInSeconds()const{
    uint64_t frame=currentFrame.load();
    if (audioFile.playbackInfo.sampleRate==0)return 0.0;
    return static_cast<double>(frame) / static_cast<double>(audioFile.playbackInfo.sampleRate);
  }

  private:
  bool loadAudioFile(const std::string& path){
    audioFile={}; // Reset all fields

    if(!std::filesystem::exists(path)){
      std::cerr << "File not found: " << path << std::endl;
      return false;
    }
    audioFile.fileInfo.filePath=path;

    SF_INFO sfinfo;
    sfinfo.format=0; // Set to 0 before opening for read

    // --- Open File ---
    SNDFILE* sndfile=sf_open(path.c_str(),SFM_READ,&sfinfo);
    if(!sndfile){
      std::cerr << "Error opening file: " << sf_strerror(NULL) << std::endl;
      return false;
    }

    // --- Fill FileInfo (Partial) ---
    audioFile.fileInfo.fileSizeBytes=std::filesystem::file_size(path);

    int major_format=sfinfo.format & SF_FORMAT_TYPEMASK;
    int minor_format=sfinfo.format & SF_FORMAT_SUBMASK;

    switch(major_format){
        case SF_FORMAT_WAV:  audioFile.fileInfo.format="wav";break;
        case SF_FORMAT_FLAC: audioFile.fileInfo.format="flac";break;
        case SF_FORMAT_OGG:  audioFile.fileInfo.format="ogg";break;
        case SF_FORMAT_MPEG: audioFile.fileInfo.format="mp3";break;
        case SF_FORMAT_OPUS: audioFile.fileInfo.format="opus";break;
        default:             audioFile.fileInfo.format="unknown";break;
    }

    // --- Fill PlaybackInfo ---
    audioFile.playbackInfo.sampleRate=sfinfo.samplerate;
    audioFile.playbackInfo.numChannels=sfinfo.channels;
    audioFile.playbackInfo.durationSeconds=static_cast<double>(sfinfo.frames) / sfinfo.samplerate;

    // --- Fill CodecInfo ---
    audioFile.codecInfo.codecName="libsndfile ("+audioFile.fileInfo.format+")";

    audioFile.codecInfo.extra["major"]=std::to_string(major_format);
    audioFile.codecInfo.extra["minor"]=std::to_string(minor_format);

    audioFile.codecInfo.isVBR=(audioFile.fileInfo.format=="mp3" || audioFile.fileInfo.format=="ogg");

    // Decode samples
    audioFile.decoded.totalFrames=sfinfo.frames;
    sf_count_t total_samples=sfinfo.frames * sfinfo.channels;
    audioFile.decoded.samples.resize(total_samples);

    sf_count_t read_frames=sf_readf_float(sndfile,audioFile.decoded.samples.data(),sfinfo.frames);

    if(read_frames != sfinfo.frames){
      audioFile.decoded.samples.resize(read_frames * sfinfo.channels);
      audioFile.decoded.totalFrames=read_frames;
    }

    // --- Simple Analysis ---
    if(!audioFile.decoded.samples.empty()){
      auto minmax_pair=std::minmax_element(audioFile.decoded.samples.begin(),audioFile.decoded.samples.end());
      audioFile.analysis.minAmplitude = *minmax_pair.first;
      audioFile.analysis.maxAmplitude = *minmax_pair.second;

      // Calculate RMS
      double sumSq=0.0;
      for(float s:audioFile.decoded.samples)sumSq += s * s;
      audioFile.analysis.rmsAmplitude=std::sqrt(sumSq / audioFile.decoded.samples.size());

      // Clipping detection (normalized float range is [-1.0, 1.0])
      audioFile.analysis.clippingDetected=(audioFile.analysis.maxAmplitude>=0.999f || audioFile.analysis.minAmplitude <= -0.999f);
    }

    // tags
    if(sndfile){
      audioFile.tags.title=getStringTag(sndfile,SF_STR_TITLE);
      audioFile.tags.artist=getStringTag(sndfile,SF_STR_ARTIST);
      audioFile.tags.album=getStringTag(sndfile,SF_STR_ALBUM);
      audioFile.tags.year=getStringTag(sndfile,SF_STR_DATE);

      audioFile.tags.extra["comment"]=getStringTag(sndfile,SF_STR_COMMENT);
      audioFile.tags.extra["genre"]=getStringTag(sndfile,SF_STR_GENRE);
      audioFile.tags.extra["tracknumber"]=getStringTag(sndfile,SF_STR_TRACKNUMBER);
    }
    sf_close(sndfile);
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
          self->state.store(PlaybackState::Playing);
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
