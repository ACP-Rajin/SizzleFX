#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

struct WAVHeader{
  char chunkID[4];       // "RIFF"
  uint32_t chunkSize;
  char format[4];        // "WAVE"
  char subchunk1ID[4];   // "fmt "
  uint32_t subchunk1Size;
  uint16_t audioFormat;  // 1 = PCM
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char subchunk2ID[4];   // "data"
  uint32_t subchunk2Size;
};

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

  // Helpers
  inline size_t getSampleCount()const{return samples.size();}
  inline size_t getSamplesPerChannel()const{return numChannels ? samples.size() / numChannels : 0;}
  inline double getDuration()const{return sampleRate ? static_cast<double>(totalFrames) / sampleRate : 0.0;}

};
class Audio{
public:
  int SINE_WAVE=0;
  int SQUARE_WAVE=1;
  int SAWTOOTH_WAVE=2;

  AudioMetadata am;

  WAVHeader header;// Optional for testing

  Audio(std::string path){
    loadWAV(path,am);
  }

private:
  bool loadWAV(const std::string& filePath,AudioMetadata& metadata){
    std::ifstream file(filePath,std::ios::binary);

    if(!file){
      std::cerr << "Error: Could not open file " << filePath << "\n";
      return false;
    }

    file.read(reinterpret_cast<char*>(&header),sizeof(WAVHeader));

    if(!file){
      std::cerr << "Error: Could not read WAV header\n";
      return false;
    }

    // Check header validity
    if(std::string(header.chunkID,4)!="RIFF"||std::string(header.format,4)!="WAVE"){
      std::cerr << "Error: Not a valid WAV file\n";
      return false;
    }

    // Fill metadata basics
    metadata.filePath=filePath;
    metadata.format=header.format;
    file.seekg(0,std::ios::end);
    metadata.fileSizeBytes=file.tellg();
    file.seekg(sizeof(WAVHeader),std::ios::beg);

    metadata.audioFormatCode=header.audioFormat;
    metadata.numChannels=header.numChannels;
    metadata.sampleRate=header.sampleRate;
    metadata.byteRate=header.byteRate;
    metadata.bitsPerSample=header.bitsPerSample;
    metadata.littleEndian=true; // WAV is little-endian

    // Figure out sample type
    switch(header.bitsPerSample){
      case 8:  metadata.sampleType=AudioMetadata::SampleType::INT8;break;
      case 16: metadata.sampleType=AudioMetadata::SampleType::INT16;break;
      case 24: metadata.sampleType=AudioMetadata::SampleType::INT24;break;
      case 32: metadata.sampleType=(header.audioFormat==3)?AudioMetadata::SampleType::FLOAT32:AudioMetadata::SampleType::INT32;break;
      default: std::cerr << "Unsupported bit depth: " << header.bitsPerSample << "\n";
               return false;
    }

    // Audio data reading
    size_t bytesPerSample=header.bitsPerSample/8;
    size_t totalSamples=header.subchunk2Size/bytesPerSample;
    metadata.totalSamples=totalSamples;
    metadata.totalFrames=totalSamples/header.numChannels;

    // Read raw data
    metadata.rawData.resize(header.subchunk2Size);
    file.read(reinterpret_cast<char*>(metadata.rawData.data()),header.subchunk2Size);

    if(!file){
      std::cerr << "Error: Could not read WAV data\n";
      return false;
    }

    // Convert to normalized float samples [-1, 1]
    metadata.samples.resize(totalSamples);
    const uint8_t* raw=metadata.rawData.data();

    if (header.bitsPerSample==8){
      for(size_t i=0;i<totalSamples;++i){
        metadata.samples[i]=(static_cast<int>(raw[i])-128)/128.0f;
      }
    }else if(header.bitsPerSample==16){
      for(size_t i=0;i<totalSamples;++i){
        int16_t sample=*reinterpret_cast<const int16_t*>(raw + i * 2);
        metadata.samples[i]=sample/32768.0f;
      }
    }else if(header.bitsPerSample==24){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample=(raw[i*3+2] << 24)|(raw[i*3+1] << 16)|(raw[i*3] << 8);
        sample >>= 8;
        metadata.samples[i]=sample/8388608.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==1){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample= *reinterpret_cast<const int32_t*>(raw + i * 4);
        metadata.samples[i]=sample / 2147483648.0f;
      }
    }else if(header.bitsPerSample==32 && header.audioFormat==3){
      for(size_t i=0;i<totalSamples;++i){
        metadata.samples[i]=*reinterpret_cast<const float*>(raw + i * 4);
      }
    }

    // Amplitude analysis
    metadata.minAmplitude=1.0f;
    metadata.maxAmplitude=-1.0f;
    double sumSquares=0.0;
    for(float s:metadata.samples){
      if(s<metadata.minAmplitude)metadata.minAmplitude=s;
      if(s>metadata.maxAmplitude)metadata.maxAmplitude=s;
      sumSquares += s * s;
    }
    metadata.rmsAmplitude=static_cast<float>(std::sqrt(sumSquares / metadata.samples.size()));
    metadata.clippingDetected=(metadata.maxAmplitude >= 0.99f || metadata.minAmplitude <= -0.99f);

    // Duration
    metadata.durationSeconds=static_cast<double>(metadata.totalFrames) / metadata.sampleRate;

    // No avgFrequency calculation yet (requires FFT)
    metadata.avgFrequency=0.0;

    // No compression info (WAV PCM is uncompressed)
    metadata.isVBR=false;
    metadata.bitrateKbps=static_cast<uint32_t>(metadata.byteRate * 8 / 1000);

    return true;
  }
};



// class Audio{
// public:
//   std::string filePath;
//   std::string fileFormat;
//
//   uint32_t sampleRate=0;
//   uint16_t bitDepth=0;
//   uint16_t channels=0;
//   uint64_t totalSamples=0;
//   float durationSeconds=0.0f;
//
//   float minAmplitude=0.0f;
//   float maxAmplitude=0.0f;
//   float averageAmplitude=0.0f;
//   float rmsAmplitude=0.0f;
//
//   uint32_t bitrate=0;
//   uint64_t fileSizeBytes=0;
//   uint64_t dataChunkSize=0;
//
//   std::vector<float>frequencySpectrum;
//
//   bool isCompressed=false;
//   bool isValid=false;
//
//   int SINE_WAVE=0;
//   int SQUARE_WAVE=1;
//   int SAWTOOTH_WAVE=2;
//
//   Audio(std::string path){
//     parseWAV();
//   }
//
// private:
//   void parseWAV(){
//     std::ifstream file(filePath,std::ios::binary);
//     if(!file){
//       std::cerr<<"Error: Could not open file "<<filePath<<"\n";
//       return;
//     }
//
//     // Get file size
//     file.seekg(0,std::ios::end);
//     fileSizeBytes=file.tellg();
//     file.seekg(0,std::ios::beg);
//
//     // Read header
//     char riff[4];
//     file.read(riff,4);
//     if(std::strncmp(riff,"RIFF",4)!=0){
//       std::cerr<<"Error: Not a valid RIFF file.\n";
//       return;
//     }
//
//     file.seekg(8,std::ios::beg);//Skip RIFF size
//     char wave[4];
//     file.read(wave,4);
//     if(std::strncmp(wave,"WAVE",4)!=0){
//       std::cerr<<"Error: Not a valid WAVE file.\n";
//       return;
//     }
//
//     //Read chunks until we find "fmt "
//     char chunkId[4];
//     uint32_t chunkSize;
//     while(file.read(chunkId,4)){
//       file.read(reinterpret_cast<char*>(&chunkSize),4);
//
//       if(std::strncmp(chunkId,"fmt ",4)==0){
//         uint16_t audioFormat;
//         file.read(reinterpret_cast<char*>(&audioFormat),2);
//         file.read(reinterpret_cast<char*>(&channels),2);
//         file.read(reinterpret_cast<char*>(&sampleRate),4);
//
//         uint32_t byteRate;
//         file.read(reinterpret_cast<char*>(&byteRate),4);
//
//         uint16_t blockAlign;
//         file.read(reinterpret_cast<char*>(&blockAlign),2);
//         file.read(reinterpret_cast<char*>(&bitDepth),2);
//
//         isCompressed=(audioFormat!=1);//PCM=1
//
//         file.seekg(chunkSize-16,std::ios::cur);//skip extra fmt data
//       }else if(std::strncmp(chunkId,"data",4)==0){
//         dataChunkSize=chunkSize;
//         break;
//       }else{
//         file.seekg(chunkSize,std::ios::cur);
//       }
//     }
//
//     // Calculate derived values
//     totalSamples=dataChunkSize / (bitDepth / 8);
//     durationSeconds=(float)totalSamples / (sampleRate * channels);
//     bitrate=sampleRate * channels * bitDepth;
//
//     isValid=true;
//     fileFormat="wav";
//   }
// };
