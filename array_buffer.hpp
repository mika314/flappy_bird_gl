#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class ArrayBuffer
{
public:
  ArrayBuffer(const GLfloat *data, int size, int cnum, int attribute);
  ArrayBuffer(const glm::vec3 *data, int size, int attribute);
  ArrayBuffer(const glm::vec2 *data, int size, int attribute);
  ArrayBuffer(const GLint *data, int size, int cnum, int attribute);
  ~ArrayBuffer();
  void activate();

private:
  GLuint id;
  GLuint attribute;
  int cNum;
  GLenum type;
};
