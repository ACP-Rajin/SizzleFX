#ifndef SIZZLEFX_CORE_AUDIO_ENGINE_HPP
#define SIZZLEFX_CORE_AUDIO_ENGINE_HPP

#include <vector>
#include <string>

namespace SizzleFX{
namespace Core{

  class AudioEngine{
    public:
      bool loadWAV(const std::string& path);
      const std::vector<float>& getSamples()const;

    private:
      std::vector<float> samples;// mono/left channel only for now
  };
}
}

#endif
