#pragma once

#include <string>
#include <vector>
#include <ncurses.h>

#include "../utils/math.hpp"

static const std::vector<std::string>waveformLevels={" ","▁","▂","▃","▄","▅","▆","▇","█"};

class UI{
  protected:
  g3dl_math::Vector2i pos_m,size_m;
  short colorPairID_m=1;
  bool highlighted_m=false;

  public:
  enum class ColorMode{
    DEFAULT, // automatically choose the best
    BASIC,   // 8-color ncurses
    EXTENDED // 256-color ncurses
  };

  virtual void draw(WINDOW* window)=0;

  // Setters
  virtual void setPosition(int x,int y){pos_m.set(x,y);}
  virtual void setPosition(g3dl_math::Vector2i& position){pos_m.set(position);}
  virtual void setX(int x){pos_m.x=x;}
  virtual void setY(int y){pos_m.y=y;}

  virtual void setSize(int width,int height){size_m.set(width,height);}
  virtual void setSize(g3dl_math::Vector2i& size){size_m.set(size);}
  virtual void setWidth(int width){size_m.x=width;}
  virtual void setHeight(int height){size_m.y=height;}

  virtual void setColorPairID(short colorPairID){colorPairID_m=colorPairID;}
  virtual void setHighlighted(bool highlighted){highlighted_m=highlighted;}

  // Getters
  virtual g3dl_math::Vector2i getPosition()const{return pos_m;}
  virtual g3dl_math::Vector2i getX()const{return pos_m.x;}
  virtual g3dl_math::Vector2i getY()const{return pos_m.y;}

  virtual g3dl_math::Vector2i getSize()const{return size_m;}
  virtual g3dl_math::Vector2i getWidth()const{return size_m.x;}
  virtual g3dl_math::Vector2i getHeight()const{return size_m.y;}

  virtual short getColorPairID()const{return colorPairID_m;}
  virtual bool getHighlighted()const{return highlighted_m;}

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

  static void initColor(short colorPairID,const g3dl_math::Vector3i& foregroundColor,const g3dl_math::Vector3i& backgroundColor,ColorMode mode=ColorMode::DEFAULT){
    if(mode==ColorMode::DEFAULT){
      const char* colorterm=getenv("COLORTERM");
      if(colorterm){
        init_pair(colorPairID,rgbTo256Color(foregroundColor),rgbTo256Color(backgroundColor));
      }else{
        init_pair(colorPairID,rgbToBasicColor(foregroundColor),rgbToBasicColor(backgroundColor));
      }
    }else if(mode==ColorMode::BASIC){
      init_pair(colorPairID,rgbToBasicColor(foregroundColor),rgbToBasicColor(backgroundColor));
    }else if(mode==ColorMode::EXTENDED && COLORS>=256){
      init_pair(colorPairID,rgbTo256Color(foregroundColor),rgbTo256Color(backgroundColor));
    }else{
      init_pair(colorPairID,COLOR_WHITE,COLOR_BLACK); // default
    }
  }

};

class Rectangle : public UI{
  public:
  struct Border{
    std::string th,bh,lv,rv,tl,tr,bl,br;
  };

  private:
  std::string character;
  bool hasBorder;
  Border bord;

  public:
  Rectangle(const g3dl_math::Vector2i& position,const g3dl_math::Vector2i& size){
    pos_m.set(position);
    size_m.set(size);
    highlighted_m=false;
    character="█";
    hasBorder=false;

    setBorderChars("┌","┐","└","┘","─","─","│","│");
  }
  void draw(WINDOW* window)override{
    int startX=pos_m.x;
    int startY=pos_m.y;
    int width=size_m.x;
    int height=size_m.y;

    wattron(window,COLOR_PAIR(colorPairID_m));
    if(highlighted_m)wattron(window,A_REVERSE);

    // --- Draw with or without border ---
    if(hasBorder && width>=2 && height>=2){
      // Top
      mvwprintw(window,startY,startX,"%s",bord.tl.c_str());
      for(int i=1;i<width-1;i++)mvwprintw(window,startY,startX+i,"%s",bord.th.c_str());
      mvwprintw(window,startY,startX+width-1,"%s",bord.tr.c_str());

      // Middle rows
      for(int j=1;j<height-1;j++){
        mvwprintw(window,startY+j,startX,"%s",bord.lv.c_str());
        //fill inside rect
        if(character!=" " && character!="")
          for(int i=1;i<width-1;i++)
            mvwprintw(window,startY+j,startX+i,"%s",character.c_str());
        mvwprintw(window,startY+j,startX+width-1,"%s",bord.rv.c_str());
      }

      // Bottom
      mvwprintw(window,startY+height-1,startX,"%s",bord.bl.c_str());
      for(int i=1;i<width-1;i++)mvwprintw(window,startY+height-1,startX+i,"%s",bord.bh.c_str());
      mvwprintw(window,startY+height-1,startX+width-1,"%s",bord.br.c_str());

    }else{
      // No border: fill entire rect
      if(character!=" " && character!="")
        for(int j=0;j<height;j++)
          for(int i=0;i<width;i++)
            mvwprintw(window,startY+j,startX+i,"%s",character.c_str());
    }

    // --- Restore attributes ---
    if(highlighted_m)wattroff(window,A_REVERSE);
    wattroff(window,COLOR_PAIR(colorPairID_m));
  }

  void setFillCharacter(std::string fillCharacter){character=fillCharacter;}
  void setHasBorder(bool hasBorder){this->hasBorder=hasBorder;}
  void setBorderChars(const std::string& top_left,const std::string& top_right,const std::string& bottom_left,const std::string& bottom_right,const std::string& top_horizontal,const std::string& bottom_horizontal,const std::string& left_vertical,const std::string& right_vertical){
    bord.tl=top_left;
    bord.tr=top_right;
    bord.bl=bottom_left;
    bord.br=bottom_right;
    bord.th=top_horizontal;
    bord.bh=bottom_horizontal;
    bord.lv=left_vertical;
    bord.rv=right_vertical;
  }
  void setBorderChars(Border& border){
    bord.tl=border.tl;
    bord.tr=border.tr;
    bord.bl=border.bl;
    bord.br=border.br;
    bord.th=border.th;
    bord.bh=border.bh;
    bord.lv=border.lv;
    bord.rv=border.rv;
  }

  std::string getFillCharacter()const{return character;}
  bool getHasBorder()const{return hasBorder;}
  Border getBorder()const{return bord;}
};

class Button : public UI{
  Rectangle rect;
  std::string label;

public:
  Button(const g3dl_math::Vector2i& position,const g3dl_math::Vector2i& size,const std::string& text):rect(position,size){
    rect.setColorPairID(colorPairID_m);
    rect.setFillCharacter(" ");
    rect.setHasBorder(true);

    pos_m.set(position);
    size_m.set(size);
    highlighted_m=false;
    label=text;
  }

  void draw(WINDOW* window)override{
    int startX=pos_m.x;
    int startY=pos_m.y;
    int width=size_m.x;
    int height=size_m.y;
    rect.setPosition(pos_m);
    rect.setSize(size_m);
    // rect.setColor(borderColor);

    rect.draw(window);

    // --- Draw label ---
    wattron(window,COLOR_PAIR(colorPairID_m));
    if(highlighted_m)wattron(window,A_REVERSE);

    std::string clipped=label.substr(0,width-2); // prevent overflow
    int text_x=startX+(width-(int)clipped.size())/2;
    int text_y=startY+height/2;

    wattron(window,COLOR_PAIR(colorPairID_m));
    mvwprintw(window,text_y,text_x,"%s",clipped.c_str());
    wattroff(window,COLOR_PAIR(colorPairID_m));

    if(highlighted_m)wattroff(window,A_REVERSE);
    wattroff(window,COLOR_PAIR(colorPairID_m));
  }

  void setLabel(const std::string &label){this->label=label;}

  std::string getLabel()const{return label;}
  Rectangle getRectangle()const{return rect;}
};
