#pragma once

#include <string>

std::string getFileContent(const char* filePath);
void dumpGLInfo();
void checkOpenGLError(const std::string& description = "");
