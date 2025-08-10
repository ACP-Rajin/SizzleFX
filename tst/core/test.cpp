#include <cassert>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

struct WAVHeaderRaw{
  char chunkID[4];     // "RIFF"
  uint32_t chunkSize;
  char format[4];      // "WAVE"
  char subchunk1ID[4]; // "fmt "
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char subchunk2ID[4];    // "data"
  uint32_t subchunk2Size; // size of the audio data in bytes
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

  Audio(std::string path){
    loadWAV(path,am);
  }

private:
  bool loadWAV(const std::string& path,AudioMetadata& meta){
    std::ifstream file(path,std::ios::binary);

    if(!file){
      std::cerr << "Error: Could not open file " << path << "\n";
      return false;
    }

    WAVHeaderRaw hdr;
    file.read(reinterpret_cast<char*>(&hdr),sizeof(WAVHeaderRaw));

    if(!file){
      std::cerr << "Error: Could not read WAV header\n";
      return false;
    }
    // Check header validity
    if(std::string(hdr.chunkID,4)!="RIFF"||std::string(hdr.format,4)!="WAVE"){
      std::cerr << "Error: Not a valid WAV file\n";
      return false;
    }

    // Fill metadata basics
    meta.filePath=path;
    meta.format="wav";
    file.seekg(0,std::ios::end);
    meta.fileSizeBytes=file.tellg();
    file.seekg(sizeof(WAVHeaderRaw),std::ios::beg);

    meta.audioFormatCode=hdr.audioFormat;
    meta.numChannels=hdr.numChannels;
    meta.sampleRate=hdr.sampleRate;
    meta.byteRate=hdr.byteRate;
    meta.bitsPerSample=hdr.bitsPerSample;
    meta.littleEndian=true; // WAV is little-endian

    // Figure out sample type
    switch(hdr.bitsPerSample){
      case 8:  meta.sampleType=AudioMetadata::SampleType::INT8;break;
      case 16: meta.sampleType=AudioMetadata::SampleType::INT16;break;
      case 24: meta.sampleType=AudioMetadata::SampleType::INT24;break;
      case 32: meta.sampleType=(hdr.audioFormat==3)?AudioMetadata::SampleType::FLOAT32:AudioMetadata::SampleType::INT32;break;
      default: std::cerr << "Unsupported bit depth: " << hdr.bitsPerSample << "\n";
               return false;
    }

    // Audio data reading
    size_t bytesPerSample=hdr.bitsPerSample/8;
    size_t totalSamples=hdr.subchunk2Size/bytesPerSample;
    meta.totalSamples=totalSamples;
    meta.totalFrames=totalSamples/hdr.numChannels;

    // Read raw data
    meta.rawData.resize(hdr.subchunk2Size);
    file.read(reinterpret_cast<char*>(meta.rawData.data()),hdr.subchunk2Size);

    if(!file){
      std::cerr << "Error: Could not read WAV data\n";
      return false;
    }

    // Convert to normalized float samples [-1, 1]
    meta.samples.resize(totalSamples);
    const uint8_t* raw=meta.rawData.data();

    if (hdr.bitsPerSample==8){
      for(size_t i=0;i<totalSamples;++i){
        meta.samples[i]=(static_cast<int>(raw[i])-128)/128.0f;
      }
    }else if(hdr.bitsPerSample==16){
      for(size_t i=0;i<totalSamples;++i){
        int16_t sample=*reinterpret_cast<const int16_t*>(raw + i * 2);
        meta.samples[i]=sample/32768.0f;
      }
    }else if(hdr.bitsPerSample==24){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample=(raw[i*3+2] << 24)|(raw[i*3+1] << 16)|(raw[i*3] << 8);
        sample >>= 8;
        meta.samples[i]=sample/8388608.0f;
      }
    }else if(hdr.bitsPerSample==32 && hdr.audioFormat==1){
      for(size_t i=0;i<totalSamples;++i){
        int32_t sample= *reinterpret_cast<const int32_t*>(raw + i * 4);
        meta.samples[i]=sample / 2147483648.0f;
      }
    }else if(hdr.bitsPerSample==32 && hdr.audioFormat==3){
      for(size_t i=0;i<totalSamples;++i){
        meta.samples[i]=*reinterpret_cast<const float*>(raw + i * 4);
      }
    }

    // Amplitude analysis
    meta.minAmplitude=1.0f;
    meta.maxAmplitude=-1.0f;
    double sumSquares=0.0;
    for(float s:meta.samples){
      if(s<meta.minAmplitude)meta.minAmplitude=s;
      if(s>meta.maxAmplitude)meta.maxAmplitude=s;
      sumSquares += s * s;
    }
    meta.rmsAmplitude=static_cast<float>(std::sqrt(sumSquares / meta.samples.size()));
    meta.clippingDetected=(meta.maxAmplitude >= 0.99f || meta.minAmplitude <= -0.99f);

    // Duration
    meta.durationSeconds=static_cast<double>(meta.totalFrames) / meta.sampleRate;

    // No avgFrequency calculation yet (requires FFT)
    meta.avgFrequency=0.0;

    // No compression info (WAV PCM is uncompressed)
    meta.isVBR=false;
    meta.bitrateKbps=static_cast<uint32_t>(meta.byteRate * 8 / 1000);

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
