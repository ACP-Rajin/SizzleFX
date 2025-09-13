#pragma once

#include <vector>
#include <ncurses.h>

#include "../utils/math.hpp"

static const std::vector<std::string>waveformLevels={" ","▁","▂","▃","▄","▅","▆","▇","█"};

class UI{
  private:
  static g3dl_math::Vector2i pos,size;
  static g3dl_math::Vector3i col;

  public:
  UI(){
    pos.set(0);
    size.set(1);
  }
  UI(int x,int y,int width,int height,int r,int g,int b){
    pos.set(x,y);
    size.set(width,height);
    col.set(r,g,b);
  }
  UI(g3dl_math::Vector2i& position,g3dl_math::Vector2i& size,g3dl_math::Vector3i& color){
    pos.set(position);
    this->size.set(size);
    col.set(color);
  }

  void setPosition(int x,int y){}
  void setPosition(g3dl_math::Vector2i& position){pos.set(pjolsition);}

  void setSize(int width,int height){}
  void setSize(g3dl_math::Vector2i& size){this->size.set(size);}

  void setColor(int r,int g,int b){}
  void setColor(g3dl_math::Vector3i& color){col.set(color);}

  g3dl_math::Vector2i getPosition(){return pos;}
  g3dl_math::Vector2i getSize(){return size;}
  g3dl_math::Vector3i getColor(){return col;}
};

class Rectangle : UI{

  Rectangle(){
  }
  Rectangle(int x,int y,int width,int height,int r,int g,int b){
  }
  Rectangle(g3dl_math::Vector2i& position,g3dl_math::Vector2i& size,g3dl_math::Vector3i& color){
  }

};
