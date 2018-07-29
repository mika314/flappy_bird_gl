#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

class ArrayBuffer
{
public:
  ArrayBuffer(const GLfloat *data, int size, int cnum, int attribute);
  ArrayBuffer(const GLint *data, int size, int cnum, int attribute);
  ~ArrayBuffer();
  void activate();

private:
  GLuint id;
  GLuint attribute;
  int cNum;
  GLenum type;
};
