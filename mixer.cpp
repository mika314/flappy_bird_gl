#include "mixer.hpp"
#include <algorithm>

static SDL_AudioSpec *desired()
{
  static SDL_AudioSpec res;
  res.freq = 44100;
  res.format = AUDIO_S16;
  res.channels = 1;
  res.samples = 4096;
  return &res;
}

Mixer::Mixer()
  : audio(nullptr, false, desired(), nullptr, 0, [&](Uint8 *stream, int len) {
      onNeedData(stream, len);
    })
{
  audio.pause(false);
}

void Mixer::play(const std::vector<int16_t> &wav)
{
  audio.lock();
  playBuf.resize(std::max(wav.size(), playBuf.size()));
  std::transform(wav.begin(), wav.end(), playBuf.begin(), playBuf.begin(), std::plus<int16_t>());
  audio.unlock();
}

void Mixer::onNeedData(Uint8 *stream, int len)
{
  for (; len > 0; len -= sizeof(int16_t), stream += sizeof(int16_t))
  {
    if (!playBuf.empty())
    {
      *(int16_t *)stream = playBuf.front();
      playBuf.pop_front();
    }
    else
      *(int16_t *)stream = 0;
  }
}
