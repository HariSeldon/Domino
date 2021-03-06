#pragma once

#include <glm/fwd.hpp>

#include <string>

class GLInitializer {
public:
  static const int OPENGL_DOUBLE_BUFFER = 1;
  static const int OPENGL_DEPTH_SIZE = 24;
  static const int OPENGL_MAJOR_VERSION = 3;
  static const int OPENGL_MINOR_VERSION = 3;

  static const int OPENGL_RED_SIZE = 4;
  static const int OPENGL_GREEN_SIZE = 4;
  static const int OPENGL_BLUE_SIZE = 4;
  static const int OPENGL_ALPHA_SIZE = 4;

  static const int OPENGL_MULTISAMPLE_BUFFERS = 1;
  static const int OPENGL_MULTISAMPLE_SAMPLES = 4;
  static const int OPENGL_BUFFER_SIZE = 32;

public:
  static glm::ivec2 initSDL();
  static void initGL();
};
