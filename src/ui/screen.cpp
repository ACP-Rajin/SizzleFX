#include "screen.hpp"
#include <ncurses.h>

namespace SizzleFX{
namespace UI{
  // Characters representing different waveform heights
  static const std::vector<std::string>waveformLevels={" ","▁","▂","▃","▄","▅","▆","▇","█"};

  void initUI(){
    initscr();           //Start ncurses mode
    cbreak();            //Disable line buffering
    noecho();            //Don't echo typed characters
    curs_set(0);         //Hide cursor
    timeout(0);          //Non-blocking getch()
    keypad(stdscr,TRUE); //Enable special keys
    start_color();       //Enable color

    // Define color pairs (foreground, background)
    init_pair(1,COLOR_WHITE,COLOR_BLUE);   //Header
    init_pair(2,COLOR_BLACK,COLOR_WHITE);  //Menu
  }

  void shutdownUI(){
    endwin();//Restore terminal to normal mode
  }

  void renderMainMenu(){
    attron(COLOR_PAIR(1));
    mvprintw(1,0,"");
    for(int i=39;i<COLS;++i)mvaddch(1,i,' ');
    attroff(COLOR_PAIR(1));
  }
}
}
