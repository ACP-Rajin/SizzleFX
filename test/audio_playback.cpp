#include <cassert>
#include <cstdlib>
#include <cstddef>
#include <ncurses.h>
#include <string>
#include "core/audio.cpp"

int main(int argc,char *argv[]){
  // if(argc<2)return 1;

  Audio audio("samples/o.wav"),audio1("samples/game_over.wav");
  if(argc>1)audio.reload(static_cast<std::string>(argv[1]));

  setlocale(LC_ALL,"");
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);
  timeout(100);

  bool running=true;
  std::string status="Press 'q' to quit.";

  while(running){
    clear();
    mvprintw(0,0,"SizzleFX Audio Editor — Press Q to Quit");
    mvprintw(1,0,"[S] Play [P] Pause [R] Resume [D] Stop [E] Seek [L] Loop [←][→] Seek ±0.5s");

    mvprintw(3,0,"Status: %s",audio.getState()==Audio::PlaybackState::Playing?(audio.getIsLoop()?"Playing (Looping)":"Playing"):(audio.getIsLoop()?"Loop Ready":"Stopped"));

    mvprintw(4,0,"Position: %.2lf / %.2f sec",audio.getPositionInSeconds(),audio.getDuration());

    refresh();

    int ch=getch();
    switch(ch){
      case 'q': case 'Q': running=false;break;
      case 's': case 'S': audio.play();break;
      case 'p': case 'P': audio.pause();break;
      case 'r': case 'R': audio.resume();break;
      case 'd': case 'D': audio.stop();break;
      case 'e': case 'E': audio.setPositionInSeconds(2.0);break;
      case 'l': case 'L': audio.setIsLoop(!audio.getIsLoop());break;
      case KEY_LEFT:      audio.setPositionInSeconds(audio.getPositionInSeconds()-0.5);break;
      case KEY_RIGHT:     audio.setPositionInSeconds(audio.getPositionInSeconds()+0.5);break;
    }
  }

  delwin(stdscr);
  endwin();
  return 0;
}
