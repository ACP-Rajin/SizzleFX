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
  Tags tags;
  DecodedAudio decoded;
  Analysis analysis;
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
