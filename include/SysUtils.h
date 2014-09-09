#pragma once

#include <string>

#include <GL/glew.h>

std::string getFileContent(const std::string &filePath);
void dumpGLInfo();
void checkOpenGLError(const std::string& description = "");
void dumpTextureToFile(GLuint inputTexture);
