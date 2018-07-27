#include "gl_check_error.hpp"

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include <sstream>
#include <stdexcept>

static const char *toString(GLenum err)
{

#define _(x) \
  case x: return #x
  switch (err)
  {
    _(GL_NO_ERROR);
    _(GL_INVALID_ENUM);
    _(GL_INVALID_VALUE);
    _(GL_INVALID_OPERATION);
    _(GL_INVALID_FRAMEBUFFER_OPERATION);
    _(GL_OUT_OF_MEMORY);
    _(GL_STACK_UNDERFLOW);
    _(GL_STACK_OVERFLOW);
  }
#undef _
  return "unknown error";
}

void checkError(const char *file, int line)
{

  GLenum err = glGetError();
  if (err == GL_NO_ERROR)
    return;
  std::ostringstream strm;
  strm << file << ":" << line << " " << err << " " << toString(err);
  throw std::runtime_error(strm.str());
}
