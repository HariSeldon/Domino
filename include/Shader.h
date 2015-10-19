#pragma once

#include <GL/glew.h>

#include <string>

class Shader {
public:
  Shader(const std::string &filePath, GLenum type);
  virtual ~Shader() = 0;

public:
  GLenum getType() const;
  const std::string& getFileName() const;
  GLuint getID() const;

private:
  GLint m_shaderID = 0;
  GLenum m_type = 0;
  std::string m_fileName;
};

class VertexShader : public Shader {
public:
  VertexShader(const std::string &filePath);
};

class FragmentShader : public Shader {
public:
  FragmentShader(const std::string &filePath);
};
