#include "texture_library.hpp"
#include <fstream>

static bool fileExists(const std::string &fileName)
{
  return std::ifstream("data/" + fileName).good();
}

int TextureLibrary::lastIdx = 0;

TextureLibrary::TextureLibrary(SDL_Renderer *aRenderer) : renderer(aRenderer) {}

int TextureLibrary::toIdx(const std::string &fileName)
{
  auto it = fileToIdx.find(fileName);
  if (it == std::end(fileToIdx))
  {
    if (!fileExists(fileName + ".bmp"))
      return -1;
    textures.emplace(
      lastIdx,
      std::make_unique<sdl::Texture>(renderer, sdl::Surface("data/" + fileName + ".bmp").get()));
    bool tmp;
    std::tie(it, tmp) = fileToIdx.emplace(fileName, lastIdx);
    ++lastIdx;
  }
  return it->second;
}

sdl::Texture *TextureLibrary::get(int idx)
{
  auto it = textures.find(idx);
  if (it == std::end(textures))
    return nullptr;
  return it->second.get();
}

sdl::Texture *TextureLibrary::get(const std::string &fileName)
{
  return get(toIdx(fileName));
}
