#include "screen.hpp"
#include <ncurses.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

namespace SizzleFX{
namespace UI{
  // Characters representing different waveform heights
  static const std::vector<std::string>waveformLevels={
    " ","▁","▂","▃","▄","▅","▆","▇","█"
  };

  void initUI(){
    initscr();              // Start ncurses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo typed characters
    curs_set(0);            // Hide cursor
    timeout(0);             // Non-blocking getch()
    keypad(stdscr,TRUE);   // Enable special keys
    start_color();          // Enable color

    // Define color pairs (foreground, background)
    init_pair(1,COLOR_WHITE,COLOR_BLUE);   // Header
    init_pair(2,COLOR_BLACK,COLOR_WHITE);  // Menu
    init_pair(3,COLOR_GREEN,COLOR_BLACK);  // Waveform
    init_pair(4,COLOR_YELLOW,COLOR_BLACK); // Controls
    init_pair(5,COLOR_CYAN,COLOR_BLACK);   // Effects
    init_pair(6,COLOR_WHITE,COLOR_BLACK);  // Status
  }

  void shutdownUI(){
    endwin();// Restore terminal to normal mode
  }

  void renderHeader(){
    attron(COLOR_PAIR(1));
    mvprintw(0,0," SizzleFX Audio Editor ");
    for(int i=24;i<COLS;++i)mvaddch(0,i,' ');
    attroff(COLOR_PAIR(1));
  }

  void renderMenuBar(){
    attron(COLOR_PAIR(2));
    mvprintw(1,0," [File] [Edit] [Effects] [Playback] [Help] ");
    for(int i=39;i<COLS;++i)mvaddch(1,i,' ');
    attroff(COLOR_PAIR(2));
  }
  void renderWaveform(const std::vector<float>& samples,int scrollIndex){
    attron(COLOR_PAIR(3));

    int height=8;
    int width=COLS-2;
    int bottomRow=3+height-1;

    for(int x=0;x<width;++x){
      int index=scrollIndex+x;
      if(index>=static_cast<int>(samples.size()))break;

      float val=samples[index];
      int level=static_cast<int>((val+1.0f)/2.0f*height);
      if(level>height)level=height;
      if(level<0)level=0;

      for(int h=0;h<height;++h){
        int row=bottomRow-h;
        std::string block=(h<level)?"█":" ";
        mvprintw(row,x+1,"%s",block.c_str());
      }
    }
    attroff(COLOR_PAIR(3));
  }
  void renderControls(){
    attron(COLOR_PAIR(4));
    std::string time="Time: 00:01.200 / 00:05.000";
    std::string buttons="| Play ⏵ | Stop ⏹ | Loop 🔁 |";

    mvprintw(5,0,"%s",time.c_str());
    mvprintw(5,COLS-buttons.length()-1,"%s",buttons.c_str());
    attroff(COLOR_PAIR(4));
  }

  void renderEffectsPanel(){
    attron(COLOR_PAIR(5));
    std::string effects="Effects: [Echo: ON] [Reverb: OFF] [Pitch: +0] [Speed: 1.0x]";
    mvprintw(7,0,"%s",effects.c_str());
    attroff(COLOR_PAIR(5));
  }

  void renderStatusBar(const std::string& status){
    attron(COLOR_PAIR(6));
    std::string paddedStatus="Status: "+status;
    mvprintw(LINES-1,0,"%s",paddedStatus.c_str());
    for(int i=paddedStatus.length();i<COLS;++i)mvaddch(LINES-1,i,' ');
    attroff(COLOR_PAIR(6));
  }
  void renderUI(const std::string& statusMessage,const std::vector<float>& samples,int scrollIndex){
    clear();
    renderHeader();
    renderMenuBar();
    renderWaveform(samples,scrollIndex);
    renderControls();
    renderEffectsPanel();
    renderStatusBar(statusMessage);
    refresh();
  }

} // namespace UI
} // namespace SizzleFX
