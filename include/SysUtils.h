#pragma once

#include <chrono>
#include <string>

#include <GL/glew.h>

std::string getFileContent(const std::string &filePath);
void dumpGLInfo();
void checkOpenGLError(const std::string& description = "");
void dumpTextureToFile(GLuint inputTexture);

auto timeFunction = [](auto &&function, auto&&... params) {
  // Start timer.
  auto begin = std::chrono::system_clock::now();
  std::forward<decltype(function)>(function)(
      std::forward<decltype(params)>(params)...
      );
  // Stop timer.
  auto end = std::chrono::system_clock::now();
  // Print execution time.
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
  return duration;
};
