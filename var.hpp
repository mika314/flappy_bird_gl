#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include <glm/matrix.hpp>


class BaseVar
{
public:
  BaseVar(const char *name);
  virtual void update() = 0;
  void updateLocation(GLuint programId);

protected:
  GLint location;
  const char *name;
};

template <typename T>
void uniform(GLint location, const T &value);

template <>
void uniform<float>(GLint location, const float &value);

template <>
void uniform<int>(GLint location, const int &value);

template <>
void uniform<glm::mat4>(GLint location, const glm::mat4 &value);

template <typename T>
class Var : public BaseVar
{
public:
  Var(const char *aName, T aValue = T()) : BaseVar(aName), value(aValue) {}
  T &operator=(const T &v)
  {
    value = v;
    return value;
  }

  operator T &() { return value; }

  void update() override
  {
    uniform<T>(location, value);
  }

private:
  T value;
};
