#pragma once
#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <portaudio.h>

// File-level metadata
struct FileInfo {
  std::string filePath;
  std::string format;       // e.g. "wav", "mp3", "ogg"
  uint64_t fileSizeBytes{};
};

// Audio format / codec info
enum class SampleType{INT8,INT16,INT24,INT32,FLOAT32,FLOAT64};

struct FormatInfo{
  uint16_t audioFormatCode{}; // PCM=1, MP3=85, OGG=codec-specific
  uint16_t numChannels{};     // 1=Mono, 2=Stereo
  uint32_t sampleRate{};      // e.g. 44100 Hz
  uint32_t byteRate{};        // bytes/sec
  uint16_t bitsPerSample{};   // 8, 16, 24, 32
  uint64_t totalSamples{};    // per channel
  uint64_t totalFrames{};     // per-channel frames
  bool littleEndian{true};
  SampleType sampleType{SampleType::INT16};
};

// Derived playback info
struct PlaybackInfo {
  double durationSeconds{};   // totalSamples / sampleRate
  double avgFrequency{};      // optional spectral centroid
};

// Signal analysis
struct Analysis{
  float minAmplitude{};      
  float maxAmplitude{};
  float rmsAmplitude{};      // Root Mean Square loudness
  bool clippingDetected{false};
};

// Compression / bitrate
struct CompressionInfo{
  bool isVBR{false};          // Variable Bit Rate
  uint32_t bitrateKbps{};     // avg bitrate
};

// Optional tags (ID3/Vorbis)
struct Tags{
  std::string title;
  std::string artist;
  std::string album;
  std::string year;
};

// Raw + processed audio data
struct AudioData{
  std::vector<float>samples;   // normalized [-1, 1]
  std::vector<uint8_t>rawData; // original bytes
};

// Unified Audio Metadata struct
struct AudioFile{
  FileInfo fileInfo;
  FormatInfo formatInfo;
  PlaybackInfo playbackInfo;
  Analysis analysis;
  CompressionInfo compression;
  Tags tags;
  AudioData data;
};

#pragma pack(push, 1)
struct HeaderWAV{
  char chunkID[4];
  uint32_t chunkSize;
  char format[4];
  char subchunk1ID[4];
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char subchunk2ID[4];   // "data"
  uint32_t subchunk2Size;
};
#pragma pack(pop)
