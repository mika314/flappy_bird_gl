#pragma once
#include "array_buffer.hpp"
#include "sdlpp.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

class ObjData;

class Obj
{
public:
  Obj(SDL_Renderer *renderer, const std::string &fileName);
  void activate();
  ~Obj();

private:
  sdl::Texture texture;
  std::unique_ptr<ObjData> objData;
  ArrayBuffer vertices;
  ArrayBuffer uvs;
  ArrayBuffer normals;
  int n;
};
