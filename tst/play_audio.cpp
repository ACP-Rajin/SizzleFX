#include <portaudio.h>
#include <cmath>
#include <iostream>

// Simple sine wave test
struct AudioTest{
  std::vector<float>samples;
  int numChannels;
  int sampleRate;
};

static int audioCallback(const void *inputBuffer,void *outputBuffer,unsigned long framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData){
  AudioTest* audio=static_cast<AudioTest*>(userData);
  float *out=static_cast<float*>(outputBuffer);
  static size_t position=0;
  size_t totalSamples=audio->samples.size();

  for(unsigned long i=0;i<framesPerBuffer*audio->numChannels;++i){
    if(position<totalSamples)
      out[i]=audio->samples[position++];
    else
      out[i]=0.0f;//silence
  }

  return(position>=totalSamples)?paComplete:paContinue;
}

int main(){
  AudioTest test;
  test.numChannels=2; // stereo
  test.sampleRate=44100;

  // Generate 1 second of 440 Hz sine wave
  double frequency=440.0;
  int durationSeconds=1;
  size_t totalSamples=test.sampleRate*test.numChannels*durationSeconds;
  test.samples.resize(totalSamples);

  for(size_t i=0;i<totalSamples;i+=test.numChannels){
    float sample=std::sin((2.0 * M_PI * frequency) * (i / test.numChannels) / test.sampleRate);
    for (int ch=0;ch<test.numChannels;++ch)test.samples[i+ch]=sample;
  }

  Pa_Initialize();
  PaStream *stream;
  Pa_OpenDefaultStream(&stream,0,test.numChannels,paFloat32,test.sampleRate,256,audioCallback,&test);

  Pa_StartStream(stream);
  while(Pa_IsStreamActive(stream)==1)Pa_Sleep(50);

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  std::cout << "Playback finished." << std::endl;
  return 0;
}
