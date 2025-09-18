#include "header.hpp"
#include <functional>
#include <ncurses.h>

// JetBrainsMono Nerd Font ASCII art banner
/*
std::vector<std::string>banner={
};

// Draw one button
void drawButton(WINDOW *menuwin,int y,int x,const std::string &text,bool highlighted){
  std::string padded=" "+text+" ";
  int btn_width=padded.size();

  // Corners change based on highlight
  const char *top_left,*top_right,*bottom_left,*bottom_right,*horizontal,*vertical;
  if(highlighted){
    top_left   ="┌";top_right   ="┐";
    bottom_left="└";bottom_right="┘";
  }else{
    top_left   ="╭";top_right   ="╮";
    bottom_left="╰";bottom_right="╯";
  }
  horizontal="─";
  vertical="│";

  // Top border
  mvwprintw(menuwin,y,x,"%s",top_left);
  for(int i=0;i<btn_width;i++)mvwprintw(menuwin,y,x+1+i,"%s",horizontal);
  mvwprintw(menuwin,y,x+btn_width+1,"%s",top_right);

  // Middle row
  mvwprintw(menuwin,y+1,x,"%s",vertical);
  if(highlighted)wattron(menuwin,A_REVERSE);
  mvwprintw(menuwin,y+1,x+1,"%s",padded.c_str());  // only text row inverts
  if(highlighted)wattroff(menuwin,A_REVERSE);
  mvwprintw(menuwin,y+1,x+btn_width+1,"%s",vertical);

  // Bottom border
  mvwprintw(menuwin,y+2,x,"%s",bottom_left);
  for(int i=0;i<btn_width;i++)mvwprintw(menuwin,y+2,x+1+i,"%s",horizontal);
  mvwprintw(menuwin,y+2,x+btn_width+1,"%s",bottom_right);
}

// Function to display the menu
int mainMenu(WINDOW *menuwin,const std::vector<std::string> &options){
  int highlight=0;
  int choice=0;
  int input;

  int spacing=2;
  int banner_height=banner.size();

  while(true){
    werase(menuwin);

    int win_height,win_width;
    getmaxyx(menuwin,win_height,win_width);
    box(menuwin,0,0);

    // Draw banner below top border
    for(size_t i=0;i<banner.size();i++){
      int banner_x=(win_width-(int)banner[i].size())/2;
      mvwprintw(menuwin,2+i,banner_x,"%s",banner[i].c_str());
    }

    // Calculate vertical start for buttons
    int total_height=(int)options.size()*3+(int)options.size()*spacing;
    int start_y=(win_height-total_height)/2+banner_height;

    // Draw buttons
    for(size_t i=0;i<options.size();i++){
      std::string padded=" "+options[i]+" ";
      int btn_width=padded.size();
      int x=(win_width-(btn_width+2))/2;
      int y=start_y+i * (3+spacing);

      drawButton(menuwin,y,x,options[i],(int)i==highlight);
    }

    wrefresh(menuwin);

    input=wgetch(menuwin);
    switch(input){
      case KEY_UP:
        highlight--;
        if(highlight<0)highlight=options.size()-1;
        break;
      case KEY_DOWN:
        highlight++;
        if(highlight>=(int)options.size())highlight=0;
        break;
      case 10: // Enter
        choice=highlight;
        return choice;
    }
  }
}

int main(){
  setlocale(LC_ALL,""); // important for Unicode glyphs
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);

  bool running=true;
  while(running){
    // Full screen window
    int height,width;
    getmaxyx(stdscr,height,width);
    WINDOW *menuwin=newwin(height,width,0,0);
    keypad(menuwin,TRUE);

    std::vector<std::string>options={
      "Create New",
      "Import Existing",
      "Options",
      "Exit"
    };

    int choice=mainMenu(menuwin,options);

    clear();
    mvprintw(height/2,((width-(options[choice].length()+14))/2),"You selected: %s",options[choice].c_str());
    refresh();

    int ch=getch();
    if(ch=='q'||ch=='Q')running=false;
  }
  endwin();
  return 0;
}
*/
int main(){
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);

  int height, width;
  getmaxyx(stdscr, height, width);
  WINDOW *menuwin = newwin(height, width, 0, 0);
  keypad(menuwin, TRUE);

  bool running = true;
  while (running) {
    werase(menuwin);

    Rectangle rect1({2,2}, {15,6}, {255,0,0},
        Rectangle::FillStyle::NONE,
        Rectangle::ColorMode::BASIC);

    Rectangle rect2({20,2}, {15,6}, {120,200,255},
        Rectangle::FillStyle::REVERSE,
        Rectangle::ColorMode::EXTENDED);

    rect1.draw(menuwin);
    rect2.draw(menuwin);

    // ✅ single refresh after all widgets
    wrefresh(menuwin);

    int ch = getch();
    if (ch == 'q' || ch == 'Q') running = false;
  }

  delwin(menuwin);
  endwin();
  return 0;
}

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
