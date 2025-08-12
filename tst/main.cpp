#include <ncurses.h>
#include "core/test.cpp"

void printHeader(WAVHeader &header){
  std::cout << "WAV Header:\n Chunk ID: ";
  std::cout.write(header.chunkID,4); std::cout << "\n";
  std::cout << "  Chunk Size: " << header.chunkSize << "\n Format: ";
  std::cout.write(header.format,4); std::cout << "\n Subchunk1 ID: ";
  std::cout.write(header.subchunk1ID,4); std::cout << "\n";
  std::cout << "  Subchunk1 Size: " << header.subchunk1Size << "\n";
  std::cout << "  Audio Format: " << header.audioFormat << "\n";
  std::cout << "  Channels: " << header.numChannels << "\n";
  std::cout << "  Sample Rate: " << header.sampleRate << "\n";
  std::cout << "  Byte Rate: " << header.byteRate << "\n";
  std::cout << "  Block Align: " << header.blockAlign << "\n";
  std::cout << "  Bits Per Sample: " << header.bitsPerSample << "\n Subchunk2 ID: ";
  std::cout.write(header.subchunk2ID,4); std::cout << "\n";
  std::cout << "  Subchunk2 Size: " << header.subchunk2Size << "\n";
}
void printMetadata(Audio &metadata){
  std::cout << "Basic Info:\n";
  std::cout << "  File Path: " << metadata.filePath << '\n';
  std::cout << "  Format: " << metadata.format << '\n';
  std::cout << "  File Size: " << metadata.fileSizeBytes << " bytes\n\n";

  std::cout << "Audio Format Info:\n";
  std::cout << "  Audio Format Code: " << metadata.audioFormatCode << '\n';
  std::cout << "  Channels: " << metadata.numChannels << '\n';
  std::cout << "  Sample Rate: " << metadata.sampleRate << '\n';
  std::cout << "  Byte Rate: " << metadata.byteRate << '\n';
  std::cout << "  Bits Per Sample: " << metadata.bitsPerSample << '\n';
  std::cout << "  Total Samples:" << metadata.totalSamples << '\n';
  std::cout << "  Total Frames: " << metadata.totalFrames << '\n';
  std::cout << "  Little-Endian?: " << metadata.littleEndian << "\n\n";

  std::cout << "Time & Frequency:\n";
  std::cout << "  Duration: " << metadata.durationSeconds << " sec\n";
  std::cout << "  Average Frequency: " << metadata.avgFrequency << " Hz\n";

  std::cout << "Amplitude Analysis:\n";
  std::cout << "  Min Amplitude: " << metadata.minAmplitude << '\n';
  std::cout << "  Max Amplitude: " << metadata.maxAmplitude << '\n';
  std::cout << "  RMS Amplitude: " << metadata.rmsAmplitude << '\n';
  std::cout << "  Clipping Detected?: " << metadata.clippingDetected << "\n\n";

  std::cout << "Compression / byteRate Info:\n";
  std::cout << "  Is Variable Bit Rate (VBR)?: " << metadata.isVBR << '\n';
  std::cout << "  Bit Rate: " << metadata.bitrateKbps << " kbps\n\n";

  std::cout << "Optional Tags:\n";
  std::cout << "  Title: " << metadata.title << '\n';
  std::cout << "  Artist: " << metadata.artist << '\n';
  std::cout << "  Album: " << metadata.album << '\n';
  std::cout << "  Year: " << metadata.year << "\n\n";

  std::cout << "Others:\n";
  std::cout << "  Sample Count: " << metadata.getSampleCount() << '\n';
  std::cout << "  Samples Per Channel: " << metadata.getSamplesPerChannel() << '\n';
  std::cout << "  Duration: " << metadata.getDuration() << '\n';
}

int main(int argc,char** argv){
  if(argc<2){
    std::cerr<<"Usage: "<<argv[0]<<" <audio_file.wav>\n";
    return 1;
  }

  Audio audio(argv[1]);

  printHeader(audio.header);
  std::cout << std::endl;
  printMetadata(audio);

  return 0;
}
