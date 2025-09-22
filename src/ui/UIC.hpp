#pragma once

#include <string>
#include <vector>
#include <ncurses.h>

#include "../utils/math.hpp"

static const std::vector<std::string>waveformLevels={" ","▁","▂","▃","▄","▅","▆","▇","█"};

class UI{
  protected:
  g3dl_math::Vector2i pos,size;
  g3dl_math::Vector3i fgCol,bgCol;
  short colorPairId;
  const short defaultColorPairId=1;
  bool isHighlighted=false,useColorPairId=false;

  public:
  enum class ColorMode{
    DEFAULT, // automatically choose the best
    BASIC,   // 8-color ncurses
    EXTENDED // 256-color ncurses
  };

  virtual void draw(WINDOW* window)=0;

  virtual void initColor(short colorPairID,g3dl_math::Vector3i& foregroundColor,g3dl_math::Vector3i& backgroundColor,ColorMode& mode){
    if(mode==ColorMode::DEFAULT){
      init_pair(colorPairID,rgbToBasicColor(foregroundColor),rgbToBasicColor(backgroundColor));
    }else if(mode==ColorMode::BASIC){
      init_pair(colorPairID,rgbToBasicColor(foregroundColor),rgbToBasicColor(backgroundColor));
    }else if(mode==ColorMode::EXTENDED && COLORS>=256){
      init_pair(colorPairID,rgbTo256Color(foregroundColor),rgbTo256Color(backgroundColor));
    }else{
      init_pair(colorPairID,COLOR_WHITE,COLOR_BLACK); // default
    }
  }

  // Setters
  virtual void setPosition(int x,int y){pos.set(x,y);}
  virtual void setPosition(g3dl_math::Vector2i& position){pos.set(position);}
  virtual void setX(int x){pos.x=x;}
  virtual void setY(int y){pos.y=y;}

  virtual void setSize(int width,int height){size.set(width,height);}
  virtual void setSize(g3dl_math::Vector2i& size){this->size.set(size);}
  virtual void setWidth(int width){size.x=width;}
  virtual void setHeight(int height){size.y=height;}

  virtual void setForegroundColor(int r,int g,int b){fgCol.set(r,g,b);}
  virtual void setForegroundColor(g3dl_math::Vector3i& color){fgCol.set(color);}
  virtual void setForegroundR(int r){fgCol.x=r;}
  virtual void setForegroundG(int g){fgCol.y=g;}
  virtual void setForegroundB(int b){fgCol.z=b;}

  virtual void setBackgroundColor(int r,int g,int b){bgCol.set(r,g,b);}
  virtual void setBackgroundColor(g3dl_math::Vector3i& color){bgCol.set(color);}
  virtual void setBackgroundR(int r){bgCol.x=r;}
  virtual void setBackgroundG(int g){bgCol.y=g;}
  virtual void setBackgroundB(int b){bgCol.z=b;}

  virtual void setIsHighlight(bool highlighted){isHighlighted=highlighted;}
  virtual void setUseColorPairID(bool useColorPairID){useColorPairId=useColorPairID;}

  // Getters
  virtual g3dl_math::Vector2i getPosition()const{return pos;}
  virtual g3dl_math::Vector2i getX()const{return pos.x;}
  virtual g3dl_math::Vector2i getY()const{return pos.y;}

  virtual g3dl_math::Vector2i getSize()const{return size;}
  virtual g3dl_math::Vector2i getWidth()const{return size.x;}
  virtual g3dl_math::Vector2i getHeight()const{return size.y;}

  virtual g3dl_math::Vector3i getForegroundColor()const{return fgCol;}
  virtual g3dl_math::Vector3i getForegroundR()const{return fgCol.x;}
  virtual g3dl_math::Vector3i getForegroundG()const{return fgCol.y;}
  virtual g3dl_math::Vector3i getForegroundB()const{return fgCol.z;}

  virtual g3dl_math::Vector3i getBackgroundColor()const{return bgCol;}
  virtual g3dl_math::Vector3i getBackgroundR()const{return bgCol.x;}
  virtual g3dl_math::Vector3i getBackgroundG()const{return bgCol.y;}
  virtual g3dl_math::Vector3i getBackgroundB()const{return bgCol.z;}

  virtual bool getIsHighlight()const{return isHighlighted;}
  virtual bool getUseColorPairID()const{return useColorPairId;}

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
  struct Border{
    std::string th,bh,lv,rv,tl,tr,bl,br;
  };

  private:
  ColorMode colorMode;
  std::string character;
  bool hasBorder;
  Border bord;
  short tmpFG,tmpBG;

  public:
  Rectangle(const g3dl_math::Vector2i& position,const g3dl_math::Vector2i& size,ColorMode mode=ColorMode::BASIC){
    pos.set(position);
    this->size.set(size);
    fgCol.set(255);
    bgCol.set(0);
    isHighlighted=false;
    character="█";
    hasBorder=false;
    colorMode=mode;

    setBorderChars("┌","┐","└","┘","─","─","│","│");
  }
  void draw(WINDOW* window)override{
    int startX=pos.x;
    int startY=pos.y;
    int width=size.x;
    int height=size.y;

    // --- Apply colors ---
    if(useColorPairId){
      // Use user Specified color-pair
      initColor(colorPairId,fgCol,bgCol,colorMode);
      wattron(window,COLOR_PAIR(colorPairId));
    }else{
      // backup & dictate
      pair_content(defaultColorPairId,&tmpFG,&tmpBG);
      initColor(defaultColorPairId,fgCol,bgCol,colorMode);
      wattron(window,COLOR_PAIR(defaultColorPairId));
    }
    if(isHighlighted)wattron(window,A_REVERSE);

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
        if(character!=" ")
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
      if(character!=" ")
        for(int j=0;j<height;j++)
          for(int i=0;i<width;i++)
            mvwprintw(window,startY+j,startX+i,"%s",character.c_str());
    }

    // --- Restore attributes ---
    if(isHighlighted)wattroff(window,A_REVERSE);
    if(useColorPairId){
      // disable color
      wattroff(window,COLOR_PAIR(colorPairId));
    }else{
      // disable color & give up hostage
      wattroff(window,COLOR_PAIR(defaultColorPairId));
      init_pair(defaultColorPairId,tmpFG,tmpBG);
    }
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
  g3dl_math::Vector3i borderFGColor;
  g3dl_math::Vector3i borderBGColor;
  std::string label;
  short colorPairId;

public:
  Button(const g3dl_math::Vector2i& pos,const g3dl_math::Vector2i& size,const std::string& text) : rect(pos,size,Rectangle::ColorMode::BASIC),label(text){
    fgCol.set(255);
    bgCol.set(0);

    rect.setForegroundColor(borderFGColor);
    rect.setBackgroundColor(borderBGColor);
    rect.setHasBorder(true);
    rect.setFillCharacter(" ");
  }

  void draw(WINDOW* window)override{
    int startX=pos.x;
    int startY=pos.y;
    int width=size.x;
    int height=size.y;
    rect.setPosition(pos);
    rect.setSize(size);
    // rect.setColor(borderColor);

    rect.draw(window);

    // --- Draw label ---
    if(isHighlighted)wattron(window,A_REVERSE);

    std::string clipped=label.substr(0,width-2); // prevent overflow
    int text_x=startX+(width-(int)clipped.size())/2;
    int text_y=startY+height/2;

    wattron(window,COLOR_PAIR(colorPairId));
    mvwprintw(window,text_y,text_x,"%s",clipped.c_str());
    wattroff(window,COLOR_PAIR(colorPairId));

    if(isHighlighted)wattroff(window,A_REVERSE);
  }

  void setBorderForegroundColor(int r,int g,int b){borderFGColor.set(r,g,b);}
  void setBorderForegroundColor(g3dl_math::Vector3i& color){borderFGColor.set(color);}
  void setBorderForegroundR(int r){borderFGColor.x=r;}
  void setBorderForegroundG(int g){borderFGColor.y=g;}
  void setBorderForegroundB(int b){borderFGColor.z=b;}

  void setBorderBackgroundColor(int r,int g,int b){borderBGColor.set(r,g,b);}
  void setBorderBackgroundColor(g3dl_math::Vector3i& color){borderBGColor.set(color);}
  void setBorderBackgroundR(int r){borderBGColor.x=r;}
  void setBorderBackgroundG(int g){borderBGColor.y=g;}
  void setBorderBackgroundB(int b){borderBGColor.z=b;}

  void setLabel(std::string &label){this->label=label;}

  std::string getLabel()const{return label;}
  Rectangle getRectangle()const{return rect;}
};
