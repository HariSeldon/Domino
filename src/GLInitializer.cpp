#include "GLInitializer.h"

#include <GL/glew.h>

#include "SDL2/SDL.h"

#include <glm/vec2.hpp>

#include "SysUtils.h"

// -----------------------------------------------------------------------------
glm::ivec2 GLInitializer::initSDL() {
  int initResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  assert(initResult == 0 && "Unable to initialize SDL");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                      GLInitializer::OPENGL_MAJOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                      GLInitializer::OPENGL_MINOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GLInitializer::OPENGL_DOUBLE_BUFFER);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, GLInitializer::OPENGL_DEPTH_SIZE);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, GLInitializer::OPENGL_RED_SIZE);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, GLInitializer::OPENGL_GREEN_SIZE);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, GLInitializer::OPENGL_BLUE_SIZE);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, GLInitializer::OPENGL_ALPHA_SIZE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,
                      GLInitializer::OPENGL_MULTISAMPLE_BUFFERS);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                      GLInitializer::OPENGL_MULTISAMPLE_SAMPLES);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, GLInitializer::OPENGL_BUFFER_SIZE);

  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  int width = displayMode.w;
  int height = displayMode.h;

  assert(width != 0 && height != 0 && "Error setting width and height");
  return glm::ivec2(width, height);
}

// -----------------------------------------------------------------------------
void GLInitializer::initGL() {
  glewExperimental = GL_TRUE;
  int glewStatus = glewInit();
  assert(glewStatus == GLEW_OK && "Error initializing glew");

  glEnable(GL_DEPTH_TEST);
  checkOpenGLError("GLInitializer: glEnable-GL_DEPTH_TEST");
  glEnable(GL_BLEND);
  checkOpenGLError("GLInitializer: glEnable-GL_BLEND");
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  checkOpenGLError("GLInitializer: glBlendFunc-GL_SRC_ALPHA");
}
