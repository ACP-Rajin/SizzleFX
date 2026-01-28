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

#include <cstdint>

class Screen{
	uint32_t m_width=0,m_height=0;

  public:
  Screen();
	Screen(uint32_t width,uint32_t height);

  void set(const Screen& screen);
	void set(uint32_t width,uint32_t height);

  void setWidth(uint32_t width);
	void setHeight(uint32_t height);

  // Don't Mind this
  uint32_t& setWidth();
	uint32_t& setHeight();

	uint32_t getWidth()const;
	uint32_t getHeight()const;
};
