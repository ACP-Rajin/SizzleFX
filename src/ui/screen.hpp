#ifndef SCREEN_HPP
#define SCREEN_HPP

namespace Screen{
  void init();
  void render();
  void handleInput();
  void cleanup();
  bool shouldQuit();
}

#endif
