#pragma once
#include "sdlpp.hpp"
#include <deque>
#include <vector>

class Mixer
{
public:
  Mixer();
  void play(const std::vector<int16_t>& wav);
private:
  sdl::Audio audio;
  std::deque<int16_t> playBuf;
  void onNeedData(Uint8 *stream, int len);
};
