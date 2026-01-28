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

#include <string>

#include "../UI.hpp"

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
  Rectangle();
  Rectangle(const Vector2i& position,const Vector2i& size);

  void draw(WINDOW* window)override;

  void setFillCharacter(std::string fillCharacter);
  void setHasBorder(bool hasBorder);
  void setBorderChars(const std::string& top_left,const std::string& top_right,const std::string& bottom_left,const std::string& bottom_right,const std::string& top_horizontal,const std::string& bottom_horizontal,const std::string& left_vertical,const std::string& right_vertical);
  void setBorderChars(Border& border);

  std::string getFillCharacter()const;
  bool getHasBorder()const;
  Border getBorder()const;
};
