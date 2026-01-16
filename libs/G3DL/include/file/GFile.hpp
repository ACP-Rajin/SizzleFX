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

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#include <stdexcept>

class GFile{
  public:
  static std::string localPath,externalFilesPath;

  private:
  std::vector<char>b;
  std::string s;

  public:
  GFile();
  GFile(std::istream& inputStream);
  GFile(const std::string& path);
  GFile(std::istream& inputStream,int chunkSize);
  GFile(const std::string& path,int chunkSize);

  void redirect(std::istream& inputStream);
  void redirect(const std::string& path);
  void redirect(std::istream& inputStream,int chunkSize);
  void redirect(const std::string& path,int chunkSize);

  std::unique_ptr<std::istream>getInputStream();

  static std::vector<char> readAsBytes(const std::string& path);
  static std::vector<char> readAsBytes(const std::string& path,int chunkSize);
  static std::vector<char> readAsBytes(std::istream& inputStream);
  static std::vector<char> readAsBytes(std::istream& inputStream,int chunkSize);

  const std::vector<char>& getBytes();

  static std::string readAsString(const std::string& path);
  static std::string readAsString(std::istream& inputStream);

  static void writeToFile(const std::string& path,const std::string& content);
  static void writeToFile(const std::string& path,const std::vector<char>& content);

  std::string readString();
};
