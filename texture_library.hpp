#pragma once
#include "sdlpp.hpp"
#include <unordered_map>
#include <memory>

class TextureLibrary
{
public:
  TextureLibrary(SDL_Renderer *renderer);
  int toIdx(const std::string &fileName);
  sdl::Texture *get(int idx);
  sdl::Texture *get(const std::string &fileName);
private:
  static int lastIdx;
  std::unordered_map<std::string, int> fileToIdx;
  std::unordered_map<int, std::unique_ptr<sdl::Texture>> textures;
  SDL_Renderer *renderer;
};
