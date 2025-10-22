#include "header.hpp"

void drawBanner(const std::vector<std::string>& banner,const g3dl_math::Vector2i& screen_size,const g3dl_math::Vector2i& banner_size,int spacing_y){
  for(size_t i=0;i<banner.size();i++){
    int row=((screen_size.y-(int)banner.size())/2)+i,col=(screen_size.x-(int)banner[i].length())/2;

    // Clip if the terminal is too narrow
    if(col<0)col=0;
    if(row>=0 && row<screen_size.y)mvwprintw(stdscr,i+((screen_size.y-banner_size.y-spacing_y)/2),(screen_size.x-banner_size.x)/2,"%s",banner[i].c_str());
  }
}

struct MainMenuSettings{
  std::string banner="auto"; // none, auto (detect), small, big
  bool playBGM=true; // background music
};

struct KeyBindings{
  int up=KEY_UP;
  int down=KEY_DOWN;
  int left=KEY_LEFT;
  int right=KEY_RIGHT;
  int select=10; // Enter
  int quit='q';
};

// struct EditorSettings{
// };

struct LayoutSettings{
  bool showStatusBar=true;
  bool showSidePanel=true;
  int padding=1;
};

struct ThemeSettings{
  int colorScheme=0; // 0=default dark, 1=dark, 2=light
};

struct Settings{
  MainMenuSettings mainMenu;
  KeyBindings keys;
  // EditorSettings editor;
  LayoutSettings layout;
  ThemeSettings theme;
};

static Settings settings;

int main(){
  setlocale(LC_ALL,"");
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

  std::vector<std::string>options={
    "New",
    "Import",
    "Settings",
    "Exit"
  };

  int highlight_MainMenu=0,highlight_Settings=0;

  Audio mainMenu_bgm({0.2f,1.0f,-0.3f,1.0f,-1.0,0.2f},2,500);

  int state=0; // MainMenu=0, setting=1, editor=2
  int ch;

  bool running=true;
  while(running){
    werase(stdscr);

    getmaxyx(stdscr,height,width);

    switch(state){
      case 1:
        {
          // draw here
          box(stdscr,0,0);

          ch=wgetch(stdscr);
          switch(ch){
            case KEY_UP:
              highlight_Settings--;
              if(highlight_Settings<0)highlight_Settings=options.size()-1;
            break;
            case KEY_DOWN:
              highlight_Settings++;
              if(highlight_Settings>=(int)options.size())highlight_Settings=0;
            break;
            case 10: // Enter
            break;
          }
        }
      break;
      case 2:
        running=false;
      break;
      default: // MainMenu
        int btn_height=3;
        int btn_width=20;
        int btn_spacing=1;
        int banner_spacing=0;

        {
          int selected_banner=0; // 0=none, 1=small, 2=big
          int btn_total_height=(int)options.size() * (btn_height+btn_spacing);
          g3dl_math::Vector2i banner_size(0);

          if(width>=96 && height>=31+btn_total_height+banner_spacing){
            if(settings.mainMenu.banner!="none"){
              if(settings.mainMenu.banner=="auto"){
                if(width>=138){
                  selected_banner=2;
                }else if(width>=47){
                  selected_banner=1;
                }else selected_banner=0;
              }else if(settings.mainMenu.banner=="small"){
                selected_banner=1;
              }else if(settings.mainMenu.banner=="big"){
                selected_banner=2;
              }
            }

            switch(selected_banner){
              case 1: // small
                banner_size.x=96;
                banner_size.y=31;
                banner_spacing=2;
                drawBanner(bannerSmall,{width,height},banner_size,btn_total_height+banner_spacing);
              break;
              case 2: // big
                banner_size.x=138;
                banner_size.y=15;
                banner_spacing=4;
                drawBanner(bannerBig,{width,height},banner_size,btn_total_height+banner_spacing);
              break;
            }
          }

          int start_y=(height-btn_total_height+banner_size.y)/2;

          std::vector<Button>buttons;
          for(size_t i=0;i<options.size();i++){
            int x=(width-btn_width)/2;
            int y=start_y+i * (btn_height+btn_spacing);
            buttons.emplace_back(g3dl_math::Vector2i(x,y),g3dl_math::Vector2i(btn_width,btn_height),options[i]);
          }

          if(settings.mainMenu.playBGM)mainMenu_bgm.play();

          // Set highlight
          for(size_t i=0;i<buttons.size();i++){
            buttons[i].setHighlighted((int)i==highlight_MainMenu);
            buttons[i].setColorPairID(0);
            buttons[i].draw(stdscr);
          }

          ch=wgetch(stdscr);
          switch(ch){
            case KEY_UP:
              highlight_MainMenu--;
              if(highlight_MainMenu<0)highlight_MainMenu=options.size()-1;
            break;
            case KEY_DOWN:
              highlight_MainMenu++;
              if(highlight_MainMenu>=(int)options.size())highlight_MainMenu=0;
            break;
            case 10: // Enter
              if(options[highlight_MainMenu]=="Exit"){
                running=false;
              }else{
                clear();
                if(options[highlight_MainMenu]=="Settings"){
                  state=1;
                }else state=2;
                refresh();
              }
            break;
          }
        }
      break;
    }

    wrefresh(stdscr);

    switch(ch){
      case KEY_RESIZE: // Refresh
        {
          getmaxyx(stdscr,height,width);
          wresize(stdscr,height,width);
          mvwin(stdscr,0,0);
        }
      break;
      case 'q':case 'Q':
        running=false;
      break;
    }
  }

  delwin(stdscr);
  endwin();
  return 0;
}

/* BASIC RECTANGLE MOVEMENT
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
*/
/*
int main(int argc,char** argv){
  if(argc<2){
  }

  // Audio audio("samples/o.wav");
  Audio audio("output.wav");
  const auto& samples=audio.audioFile.decoded.samples;

  AudioPlayer player;
  player.load(audio.audioFile.decoded.samples,audio.audioFile.playbackInfo.numChannels,audio.audioFile.playbackInfo.sampleRate);

  setlocale(LC_ALL,"");
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr,TRUE);

  bool running=true;
  std::string status="Press 'q' to quit.";

  while(running){
    clear();
    mvprintw(0,0,"SizzleFX Audio Editor — Press Q to Quit");
    mvprintw(1,0,"[S]tart  [P]ause  [R]esume  [D]stop  [E] Seek  [L]oop  [←][→] Seek ±0.5s");

    mvprintw(3,0,"Status: %s",player.isPlayingNow()?(player.isLooping()?"Playing (Looping)":"Playing"):(player.isLooping()?"Loop Ready":"Stopped"));

    mvprintw(4,0,"Position: %.2lf / %.2f sec",player.getCurrentTime(),player.getDurationSeconds());

    refresh();

    int ch=getch();
    switch(ch) {
      case 'q': case 'Q': running=false;break;
      case 's': case 'S': player.start();break;
      case 'p': case 'P': player.pause();break;
      case 'r': case 'R': player.resume();break;
      case 'd': case 'D': player.stop();break;
      case 'e': case 'E': player.seek(2.0);break;
      case 'l': case 'L': player.setLoop(!player.isLooping());
                          if(player.isLooping())player.setLoopRegion(0.0,3.0);
                          break;

      case KEY_LEFT:      player.seek(player.getCurrentTime()-0.5);
                          break;

      case KEY_RIGHT:     player.seek(player.getCurrentTime()+0.5);
                          break;
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  delwin(stdscr);
  endwin();
  return 0;
};*/
