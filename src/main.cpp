#include "header.hpp"

/*
void drawBanner(WINDOW* win,const std::vector<std::string>& banner){
  int height,width;
  getmaxyx(win,height,width);

  for(size_t i=0;i<banner.size();i++){
    int row=(height-(int)banner.size())/2+i;
    int col=(width-(int)banner[i].length())/2;

    // Clip if the terminal is too narrow
    if(col<0)col=0;
    if(row>=0 && row<height){
      mvwprintw(win,row,col,"%s",banner[i].c_str());
    }
  }
}
const std::vector<std::string>& selectBanner(int width){
  // fallback if too small for even bannerSmall
  if(width<(int)bannerSmall[0].length())
    return bannerSmall;

  // use big banner if terminal is wide enough
  if(width>=(int)bannerBig[0].length())
    return bannerBig;

  // otherwise small
  return bannerSmall;
}

int main(){
  setlocale(LC_ALL,"");
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);

  std::vector<std::string>options={
    "New",
    "Import",
    "Settings",
    "Exit"
  };

  int highlight=0;

  bool running=true;
  while(running){
    werase(stdscr);

    int height,width;
    getmaxyx(stdscr,height,width);

    // Calculate vertical positioning
    int btn_height=3;
    int btn_width=20;
    int spacing=2;
    int total_height=(int)options.size() * (btn_height+spacing);
    int start_y=(height-total_height)/2;

    std::vector<Button>buttons;
    for(size_t i=0;i<options.size();i++){
      int x=(width-btn_width)/2;
      int y=start_y+i * (btn_height+spacing);
      buttons.emplace_back(g3dl_math::Vector2i(x,y),g3dl_math::Vector2i(btn_width,btn_height),options[i]);
    }

    // Set highlight
    for(size_t i=0;i<buttons.size();i++){
      buttons[i].setIsHighlight((int)i==highlight);
      buttons[i].draw(stdscr);
    }

    wrefresh(stdscr);

    int ch=getch();
    switch(ch){
      case KEY_UP:
        highlight--;
        if(highlight<0)highlight=options.size()-1;
        break;
      case KEY_DOWN:
        highlight++;
        if(highlight>=(int)options.size())highlight=0;
        break;
      case 10: // Enter
        if(options[highlight]=="Exit"){
          running=false;
        }else{
          clear();
          mvprintw(height/2,(width-(int)options[highlight].size()-14)/2,"You selected: %s",options[highlight].c_str());
          refresh();
          getch();
        }
        break;
      case 'q':case 'Q':
        running=false;
        break;
    }
  }

  endwin();
  return 0;
}
*/
/* BASIC RECTANGLE MOVEMENT*/
int main(){
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);

  int height,width;
  getmaxyx(stdscr,height,width);
  WINDOW *menuwin=newwin(height,width,0,0);
  keypad(menuwin,TRUE);

  // --- Create your widgets ---
  // Rectangle rect1({2,2},{15,6},{255,0,0},Rectangle::FillStyle::NONE,Rectangle::ColorMode::BASIC);
  // Rectangle rect2({20,2},{15,6},{120,200,255},Rectangle::FillStyle::REVERSE,Rectangle::ColorMode::EXTENDED);

  Rectangle rect1({2,2},{15,6},Rectangle::ColorMode::BASIC),
            rect2({20,2},{15,6},Rectangle::ColorMode::EXTENDED),
            rect3({38,2},{15,6},Rectangle::ColorMode::EXTENDED),
            rect4({56,2},{15,6},Rectangle::ColorMode::EXTENDED);

  Rectangle rect5({2,9},{15,6},Rectangle::ColorMode::EXTENDED),
            rect6({20,9},{15,6},Rectangle::ColorMode::EXTENDED),
            rect7({38,9},{15,6},Rectangle::ColorMode::EXTENDED),
            rect8({56,9},{15,6},Rectangle::ColorMode::EXTENDED);

  rect1.setForegroundColor(255,0,0);
  rect2.setForegroundColor(0,255,0);
  rect3.setForegroundColor(0,0,255);

  rect5.setForegroundColor(167,55,76);
  rect6.setForegroundColor(90,0,101);
  rect7.setForegroundColor(200,100,110);
  rect8.setForegroundColor(166,255,78);

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
/**/
/*
int main(int argc,char** argv){
  using namespace SizzleFX;

  if(argc<2){
  }

  std::string state="MainMenu";

  Audio audio("samples/o.wav");
  const auto& samples=audio.audioFile.decoded.samples;
  UI::initUI();
  bool running=true;
  std::string status="Press 'q' to quit.";

  while(running){
    clear();
    if(state=="MainMenu"){
      attron(COLOR_PAIR(1));
      mvprintw(0,0," SizzleFX Audio Editor ");
      for(int i=24;i<COLS;++i)mvaddch(0,i,' ');
      attroff(COLOR_PAIR(1));

      attron(COLOR_PAIR(2));
      mvprintw(1,0," move with [↑] [↓] [←] [→] ");
      mvprintw(1,0,"");
      for(int i=39;i<COLS;++i)mvaddch(1,i,' ');
      attroff(COLOR_PAIR(2));
    }
    refresh();

    int ch=getch();
    if(ch=='q'||ch=='Q')running=false;
    if(ch=='p'||ch=='P')audio.play();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  UI::shutdownUI();

  return 0;
};*/
