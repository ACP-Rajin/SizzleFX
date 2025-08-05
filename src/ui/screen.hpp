#ifndef SIZZLEFX_UI_SCREEN_HPP
#define SIZZLEFX_UI_SCREEN_HPP

#include <string>
#include <vector>

namespace SizzleFX{
namespace UI{
  void initUI();
  void shutdownUI();
  void renderUI(const std::string& statusMessage,const std::vector<float>& samples,int scrollIndex);

  // Internal components
  void renderHeader();
  void renderMenuBar();
  void renderWaveform(const std::vector<float>& samples,int scrollIndex);
  void renderControls();
  void renderEffectsPanel();
  void renderStatusBar(const std::string& status);
}
}

#endif // SIZZLEFX_UI_SCREEN_HPP
