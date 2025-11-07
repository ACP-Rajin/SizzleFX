#include "ui/UIC.hpp" // UI collection
#include "utils/math.hpp"

int main(){
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);

  UI::initColor(0,{0},{0});
  UI::initColor(1,{255,0,0},{0});
  UI::initColor(2,{0,255,0},{0});
  UI::initColor(3,{0,0,255},{0});
  UI::initColor(4,{167,55,76},{0});
  UI::initColor(5,{90,0,101},{0});
  UI::initColor(6,{200,100,110},{0});
  UI::initColor(7,{166,255,78},{0});

  int height,width;
  getmaxyx(stdscr,height,width);
  WINDOW *menuwin=newwin(height,width,0,0);
  keypad(menuwin,TRUE);

  Rectangle rect1({2,2},{15,6}),
            rect2({20,2},{15,6}),
            rect3({38,2},{15,6}),
            rect4({56,2},{15,6});

  Rectangle rect5({2,9},{15,6}),
            rect6({20,9},{15,6}),
            rect7({38,9},{15,6}),
            rect8({56,9},{15,6});

  rect1.setColorPairID(0);
  rect2.setColorPairID(1);
  rect3.setColorPairID(2);
  rect4.setColorPairID(3);

  rect5.setColorPairID(4);
  rect6.setColorPairID(5);
  rect7.setColorPairID(6);
  rect8.setColorPairID(7);

  rect2.setFillCharacter("g");
  rect3.setFillCharacter("&");
  rect4.setFillCharacter("\\");

  rect6.setFillCharacter("g");
  rect7.setFillCharacter("&");
  rect8.setFillCharacter("\\");

  rect5.setHasBorder(true);
  rect6.setHasBorder(true);
  rect7.setHasBorder(true);
  rect8.setHasBorder(true);

  bool running=true;
  while(running){
    werase(menuwin);

    rect1.draw(menuwin);
    rect2.draw(menuwin);
    rect3.draw(menuwin);
    rect4.draw(menuwin);

    rect5.draw(menuwin);
    rect6.draw(menuwin);
    rect7.draw(menuwin);
    rect8.draw(menuwin);

    wrefresh(menuwin);

    int ch=wgetch(menuwin);  // blocking wait for input
    switch(ch){
      case 'q':case'Q':
        running=false;
        break;
      case KEY_RESIZE:{
        getmaxyx(stdscr,height,width);
        wresize(menuwin,height,width);
        mvwin(menuwin,0,0);
      }break;// later: handle highlights, navigation, etc.
      case KEY_UP:
        rect1.setPosition(rect1.getPosition().x,rect1.getPosition().y-1);
        rect2.setPosition(rect2.getPosition().x,rect2.getPosition().y-1);
        rect3.setPosition(rect3.getPosition().x,rect3.getPosition().y-1);
        rect4.setPosition(rect4.getPosition().x,rect4.getPosition().y-1);

        rect5.setPosition(rect5.getPosition().x,rect5.getPosition().y-1);
        rect6.setPosition(rect6.getPosition().x,rect6.getPosition().y-1);
        rect7.setPosition(rect7.getPosition().x,rect7.getPosition().y-1);
        rect8.setPosition(rect8.getPosition().x,rect8.getPosition().y-1);
        break;
      case KEY_LEFT:
        rect1.setPosition(rect1.getPosition().x-2,rect1.getPosition().y);
        rect2.setPosition(rect2.getPosition().x-2,rect2.getPosition().y);
        rect3.setPosition(rect3.getPosition().x-2,rect3.getPosition().y);
        rect4.setPosition(rect4.getPosition().x-2,rect4.getPosition().y);

        rect5.setPosition(rect5.getPosition().x-2,rect5.getPosition().y);
        rect6.setPosition(rect6.getPosition().x-2,rect6.getPosition().y);
        rect7.setPosition(rect7.getPosition().x-2,rect7.getPosition().y);
        rect8.setPosition(rect8.getPosition().x-2,rect8.getPosition().y);
        break;
      case KEY_DOWN:
        rect1.setPosition(rect1.getPosition().x,rect1.getPosition().y+1);
        rect2.setPosition(rect2.getPosition().x,rect2.getPosition().y+1);
        rect3.setPosition(rect3.getPosition().x,rect3.getPosition().y+1);
        rect4.setPosition(rect4.getPosition().x,rect4.getPosition().y+1);

        rect5.setPosition(rect5.getPosition().x,rect5.getPosition().y+1);
        rect6.setPosition(rect6.getPosition().x,rect6.getPosition().y+1);
        rect7.setPosition(rect7.getPosition().x,rect7.getPosition().y+1);
        rect8.setPosition(rect8.getPosition().x,rect8.getPosition().y+1);
        break;
      case KEY_RIGHT:
        rect1.setPosition(rect1.getPosition().x+2,rect1.getPosition().y);
        rect2.setPosition(rect2.getPosition().x+2,rect2.getPosition().y);
        rect3.setPosition(rect3.getPosition().x+2,rect3.getPosition().y);
        rect4.setPosition(rect4.getPosition().x+2,rect4.getPosition().y);

        rect5.setPosition(rect5.getPosition().x+2,rect5.getPosition().y);
        rect6.setPosition(rect6.getPosition().x+2,rect6.getPosition().y);
        rect7.setPosition(rect7.getPosition().x+2,rect7.getPosition().y);
        rect8.setPosition(rect8.getPosition().x+2,rect8.getPosition().y);
        break;
    }
  }

  delwin(menuwin);
  endwin();
  return 0;
}
