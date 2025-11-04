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
  printf("%i",Audio("samples/game_over.wav").audioFile.playbackInfo.sampleRate);

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

  std::vector<float>sampleRate(500);
  for(size_t i=0;i<500/5;i++){
    sampleRate.emplace_back(0.2);
    sampleRate.emplace_back(1.0);
    sampleRate.emplace_back(-.3);
    sampleRate.emplace_back(1.0);
    sampleRate.emplace_back(-1);
  }
  Audio bgm_MainMenu({0.2f,1.0f,-0.3f,1.0f,-1.0},2,100);
  bgm_MainMenu.setIsLoop(true);

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

          if(settings.mainMenu.playBGM)bgm_MainMenu.play();

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
                bgm_MainMenu.stop();
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
