#pragma once

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include "var.hpp"
#include <stdexcept>
#include <vector>

using LoadFileError = std::runtime_error;
class CompileShaderError : public std::runtime_error
{
public:
  CompileShaderError(const std::string &error) : std::runtime_error(error) {}
};

class ShaderProgram
{
public:
  ShaderProgram(const std::string &vertex, const std::string &fragment);

  template <typename... Args>
  ShaderProgram(const std::string &vertex, const std::string &fragment, Args &... args)
    : vars(loadVars(args...))
  {
    init(vertex, fragment);
  }

  ~ShaderProgram();
  void use();

  // C++ boiler plate interface
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;
  ShaderProgram &operator=(ShaderProgram &&other)
  {
    if (this == &other)
      return *this;
    programId = other.programId;
    other.programId = 0xffffffffU;
    return *this;
  }

private:
  std::vector<BaseVar *> vars;
  GLuint programId;
  std::string vertexFileName;
  std::string fragmentFileName;
  time_t tsVertex = 0;
  time_t tsFragment = 0;

  void updateVars();
  GLuint loadShaders(const char *vertexFilePath, const char *fragmentFilePath);
  void init(const std::string &vertex, const std::string &fragment);

  void loadVars(std::vector<BaseVar *> &) {}

  template <typename... Args>
  std::vector<BaseVar *> loadVars(Args &... args)
  {
    std::vector<BaseVar *> res;
    loadVars(res, args...);
    return res;
  }
  template <typename Arg, typename... Args>
  void loadVars(std::vector<BaseVar *> &res, Arg &arg, Args &... args)
  {
    res.push_back(&arg);
    loadVars(res, args...);
  }
};
