#pragma once

#include <vector>
#include <ncurses.h>

#include "../utils/math.hpp"

static const std::vector<std::string>waveformLevels={" ","▁","▂","▃","▄","▅","▆","▇","█"};

class UI{
  protected:
  g3dl_math::Vector2i pos,size;
  g3dl_math::Vector3i col;
  bool isHighlighted=true;

  public:
  virtual void draw(WINDOW* window)=0;

  void setPosition(int x,int y){size.set(x,y);}
  void setPosition(g3dl_math::Vector2i& position){pos.set(position);}

  void setSize(int width,int height){size.set(width,height);}
  void setSize(g3dl_math::Vector2i& size){this->size.set(size);}

  void setColor(int r,int g,int b){col.set(r,g,b);}
  void setColor(g3dl_math::Vector3i& color){col.set(color);}

  void setIsHighlight(bool highlighted){isHighlighted=highlighted;}

  g3dl_math::Vector2i getPosition(){return pos;}
  g3dl_math::Vector2i getSize(){return size;}
  g3dl_math::Vector3i getColor(){return col;}
  bool getIsHighlight(){return isHighlighted;}

  // utils
  // --- Map RGB -> ncurses 8 colors ---
  static short rgbToBasicColor(const g3dl_math::Vector3i& rgb){
    int r=rgb.x,g=rgb.y,b=rgb.z;
    if(r>200 && g<100 && b<100)return COLOR_RED;
    if(g>200 && r<100 && b<100)return COLOR_GREEN;
    if(b>200 && r<100 && g<100)return COLOR_BLUE;
    if(r>200 && g>200 && b<100)return COLOR_YELLOW;
    if(r>200 && b>200 && g<100)return COLOR_MAGENTA;
    if(g>200 && b>200 && r<100)return COLOR_CYAN;
    if(r>200 && g>200 && b>200)return COLOR_WHITE;
    return COLOR_BLACK;
  }

  // --- Map RGB -> 256-color cube ---
  static short rgbTo256Color(const g3dl_math::Vector3i& rgb){
    int r=rgb.x * 6/256;
    int g=rgb.y * 6/256;
    int b=rgb.z * 6/256;
    return 16+(36 * r)+(6 * g)+b; // 256-color cube index
  }

};

class Rectangle : public UI{
  public:
  enum class FillStyle{
    NONE,    // border only
    SOLID,   // solid block █
    SHADED,  // checkerboard ▒
    REVERSE  // inverted space
  };

  enum class ColorMode{
    BASIC,   // 8-color ncurses
    EXTENDED // 256-color ncurses
  };

  private:
  FillStyle fillStyle;
  ColorMode colorMode;
  short colorPairId;

  public:
  Rectangle(const g3dl_math::Vector2i& position,const g3dl_math::Vector2i& newSize,const g3dl_math::Vector3i& color,FillStyle style=FillStyle::NONE,ColorMode mode=ColorMode::BASIC){
    pos.set(position);
    size.set(newSize);
    col.set(color);
    isHighlighted=false;
    fillStyle=style;
    colorMode=mode;

    static short nextId=1;

    if(mode==ColorMode::BASIC){
      short nc=rgbToBasicColor(col);
      colorPairId=nextId++;
      init_pair(colorPairId,nc,COLOR_BLACK);
    }else if(mode==ColorMode::EXTENDED && COLORS>=256){
      short nc=rgbTo256Color(col);
      colorPairId=nextId++;
      init_pair(colorPairId,nc,COLOR_BLACK);
    }else{
      colorPairId=1; // fallback
    }
  }

  void draw(WINDOW* window)override{
    int startX=pos.x;
    int startY=pos.y;
    int width=size.x;
    int height=size.y;

    wattron(window,COLOR_PAIR(colorPairId));
    if(isHighlighted)wattron(window,A_REVERSE);

    // --- Borders ---
    for(int i=0;i<width;i++){
      mvwaddch(window,startY,startX+i,ACS_HLINE);
      mvwaddch(window,startY+height-1,startX+i,ACS_HLINE);
    }
    for(int i=0;i<height;i++){
      mvwaddch(window,startY+i,startX,ACS_VLINE);
      mvwaddch(window,startY+i,startX+width-1,ACS_VLINE);
    }
    mvwaddch(window,startY,startX,ACS_ULCORNER);
    mvwaddch(window,startY,startX+width-1,ACS_URCORNER);
    mvwaddch(window,startY+height-1,startX,ACS_LLCORNER);
    mvwaddch(window,startY+height-1,startX+width-1,ACS_LRCORNER);

    // --- Fill ---
    if(fillStyle!=FillStyle::NONE && width>2 && height>2){
      for(int j=1;j<height-1;j++){
        for(int i=1;i<width-1;i++){
          chtype ch=' ';
          switch(fillStyle){
            case FillStyle::SOLID:ch=ACS_BLOCK;break;        // solid block
            case FillStyle::SHADED:ch=ACS_CKBOARD; break;    // checkerboard
            case FillStyle::REVERSE:ch=' '|A_REVERSE;break;
            case FillStyle::NONE:break;
          }
          if(fillStyle!=FillStyle::NONE)
            mvwaddch(window,startY+j,startX+i,ch);
        }
      }
    }

    if(isHighlighted)wattroff(window,A_REVERSE);
    wattroff(window,COLOR_PAIR(colorPairId));

    wrefresh(window);
  }

  void setFillStyle(FillStyle style){fillStyle=style;}
  FillStyle getFillStyle()const{return fillStyle;}
};
