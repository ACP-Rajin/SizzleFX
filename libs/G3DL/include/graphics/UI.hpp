/*
 * Copyright 2025 The G3DL Project Developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#pragma once

#include <ncurses.h>

#include "../math/Math.hpp"

class UI{
  protected:
  Vector2i pos_m,size_m;
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
  virtual void setPosition(const Vector2i& position){pos_m.set(position);}
  virtual void setX(int x){pos_m.x=x;}
  virtual void setY(int y){pos_m.y=y;}

  virtual void setSize(int width,int height){size_m.set(width,height);}
  virtual void setSize(const Vector2i& size){size_m.set(size);}
  virtual void setWidth(int width){size_m.x=width;}
  virtual void setHeight(int height){size_m.y=height;}

  virtual void setColorPairID(short colorPairID){colorPairID_m=colorPairID;}
  virtual void setHighlighted(bool highlighted){highlighted_m=highlighted;}

  // Getters
  virtual Vector2i getPosition()const{return pos_m;}
  virtual int getX()const{return pos_m.x;}
  virtual int getY()const{return pos_m.y;}

  virtual Vector2i getSize()const{return size_m;}
  virtual int getWidth()const{return size_m.x;}
  virtual int getHeight()const{return size_m.y;}

  virtual short getColorPairID()const{return colorPairID_m;}
  virtual bool getHighlighted()const{return highlighted_m;}

  // utils
  // --- Map RGB -> ncurses 8 colors ---
  static short rgbToBasicColor(const Vector3i& rgb){
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
  static short rgbTo256Color(const Vector3i& rgb){
    int r=rgb.x * 6/256;
    int g=rgb.y * 6/256;
    int b=rgb.z * 6/256;
    return 16+(36 * r)+(6 * g)+b; // 256-color cube index
  }

  static void initColor(short colorPairID,const Vector3i& foregroundColor,const Vector3i& backgroundColor,ColorMode mode=ColorMode::DEFAULT){
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
