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

#include <memory>

#include "graphics/DefaultRenderer.hpp"
#include "GameInterface.hpp"
#include "G3D.hpp"

class G3DActivity{
  std::unique_ptr<GRenderer> m_renderer;
  std::unique_ptr<GameInterface> m_gameInterface;

  public:
  G3DActivity();
  G3DActivity(std::unique_ptr<GRenderer> renderer);

	void init(std::unique_ptr<GameInterface> gameInterface);

  void setRenderer(std::unique_ptr<GRenderer> renderer);

  void onDespose();
};
