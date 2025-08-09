#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

struct AudioMetadata{
  // Basic info
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

  // Sample type
  enum class SampleType{INT8,INT16,INT24,INT32,FLOAT32,FLOAT64};
  SampleType sampleType;

  // Time & frequency
  double durationSeconds;
  double avgFrequency;

  // Amplitude analysis
  float minAmplitude;    // Lowest amplitude (-1.0f)
  float maxAmplitude;    // Highest amplitude (1.0f)
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

  //Helper methods
  inline size_t getSampleCount()const{return samples.size();}
  inline size_t getSamplesPerChannel()const{return numChannels?samples.size()/numChannels:0;}
  // Helpers
  inline size_t getSampleCount()const{return samples.size();}
  inline size_t getSamplesPerChannel()const{return numChannels ? samples.size() / numChannels : 0;}
  inline double getDuration()const{return sampleRate ? static_cast<double>(totalFrames) / sampleRate : 0.0;}
};
class Audio{
public:
  std::string filePath;
  std::string fileFormat;

  uint32_t sampleRate=0;
  uint16_t bitDepth=0;
  uint16_t channels=0;
  uint64_t totalSamples=0;
  float durationSeconds=0.0f;

  float minAmplitude=0.0f;
  float maxAmplitude=0.0f;
  float averageAmplitude=0.0f;
  float rmsAmplitude=0.0f;

  uint32_t bitrate=0;
  uint64_t fileSizeBytes=0;
  uint64_t dataChunkSize=0;

  std::vector<float>frequencySpectrum;

  bool isCompressed=false;
  bool isValid=false;

  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  Audio(std::string path){
    filePath=path;
    parseWAV();
  }

private:
  void parseWAV(){
    std::ifstream file(filePath,std::ios::binary);
    if(!file){
      std::cerr<<"Error: Could not open file "<<filePath<<"\n";
      return;
    }

    // Get file size
    file.seekg(0,std::ios::end);
    fileSizeBytes=file.tellg();
    file.seekg(0,std::ios::beg);

    // Read header
    char riff[4];
    file.read(riff,4);
    if(std::strncmp(riff,"RIFF",4)!=0){
      std::cerr<<"Error: Not a valid RIFF file.\n";
      return;
    }

    file.seekg(8,std::ios::beg);//Skip RIFF size
    char wave[4];
    file.read(wave,4);
    if(std::strncmp(wave,"WAVE",4)!=0){
      std::cerr<<"Error: Not a valid WAVE file.\n";
      return;
    }

    //Read chunks until we find "fmt "
    char chunkId[4];
    uint32_t chunkSize;
    while(file.read(chunkId,4)){
      file.read(reinterpret_cast<char*>(&chunkSize),4);

      if(std::strncmp(chunkId,"fmt ",4)==0){
        uint16_t audioFormat;
        file.read(reinterpret_cast<char*>(&audioFormat),2);
        file.read(reinterpret_cast<char*>(&channels),2);
        file.read(reinterpret_cast<char*>(&sampleRate),4);

        uint32_t byteRate;
        file.read(reinterpret_cast<char*>(&byteRate),4);

        uint16_t blockAlign;
        file.read(reinterpret_cast<char*>(&blockAlign),2);
        file.read(reinterpret_cast<char*>(&bitDepth),2);

        isCompressed=(audioFormat!=1);//PCM=1

        file.seekg(chunkSize-16,std::ios::cur);//skip extra fmt data
      }else if(std::strncmp(chunkId,"data",4)==0){
        dataChunkSize=chunkSize;
        break;
      }else{
        file.seekg(chunkSize,std::ios::cur);
      }
    }

    // Calculate derived values
    totalSamples=dataChunkSize / (bitDepth / 8);
    durationSeconds=(float)totalSamples / (sampleRate * channels);
    bitrate=sampleRate * channels * bitDepth;

    isValid=true;
    fileFormat="wav";
  }
};
