#pragma once

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include <stdexcept>

using LoadFileError = std::runtime_error;
class CompileShaderError : public std::runtime_error
{
public:
  CompileShaderError(const std::string &error) : std::runtime_error(error) {}
};

class ShaderProgram
{
public:
  ShaderProgram(const std::string& vertex, const std::string& fragment);
  ~ShaderProgram();
  GLuint get();

  // C++ boiler plate interface
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&& other)
  {
    if (this == &other)
      return *this;
    programId = other.programId;
    other.programId = 0xffffffffU;
    return *this;
  }
  
private:
  GLuint programId;
  std::string vertexFileName;
  std::string fragmentFileName;
  time_t tsVertex = 0;
  time_t tsFragment = 0;
};