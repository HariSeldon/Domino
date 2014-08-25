#pragma once

#include <string>

#include <GL/glew.h>

std::string getFileContent(const char* filePath);
void dumpGLInfo();
void checkOpenGLError(const std::string& description = "");
void dumpTextureToFile(GLuint inputTexture);
