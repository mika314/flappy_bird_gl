#include "mixer.hpp"
#include <sdlpp/sdlpp.hpp>
#include <shade/array_buffer.hpp>
#include <shade/obj.hpp>
#include <shade/shader_program.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

std::vector<int16_t> loadWav(const std::string &fileName)
{
  SDL_AudioSpec wav_spec;
  Uint32 wav_length;
  Uint8 *wav_buffer;
  if (SDL_LoadWAV(("data/" + fileName + ".wav").c_str(), &wav_spec, &wav_buffer, &wav_length) ==
      nullptr)
    std::cerr << "Could not open test.wav: " << SDL_GetError() << "\n";
  std::vector<int16_t> res(wav_length / sizeof(int16_t));
  memcpy(res.data(), wav_buffer, wav_length);
  // Do stuff with the WAV data, and then...
  SDL_FreeWAV(wav_buffer);
  return res;
}

int main() try
{
  sdl::Init init(SDL_INIT_EVERYTHING);
  const auto Width = 1280;
  const auto Height = 720;
  sdl::Window w("Flappy Bird", 64, 126, Width, Height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);

  sdl::Renderer r(w.get(), -1, SDL_RENDERER_PRESENTVSYNC);

  auto coin = loadWav("coin");
  auto explosion = loadWav("explosion");

  Mixer mixer;

  sdl::EventHandler e;
  bool done = false;
  float camY = -22;
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.mouseWheel = [&camY](const SDL_MouseWheelEvent &e) {
    if (e.y > 0)
      camY *= 1.03;
    else
      camY *= 0.97;
  };
  float birdY = 0;
  float birdX = 0;
  float birdVY = 0;
  bool gameIsStarted = false;

  e.keyDown = [&birdVY, &gameIsStarted](const SDL_KeyboardEvent &e) {
    if (e.keysym.scancode == SDL_SCANCODE_SPACE)
    {
      if (gameIsStarted)
        birdVY += 0.1f;
      else
        gameIsStarted = true;
    }
  };
  e.mouseButtonDown = [&birdVY, &gameIsStarted](const SDL_MouseButtonEvent &) {
    if (gameIsStarted)
      birdVY += 0.1f;
    else
      gameIsStarted = true;
  };

  std::vector<float> pipes;
  for (int i = 0; i < 1000; ++i)
    pipes.push_back((rand() % 200 - 100) / 20.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  Var<glm::mat4> mvp("mvp");
  Var<float> explodeTime("explodeTime");
  Var<float> pipeAngle("angle");
  ShaderProgram objShader("obj", "obj", mvp);
  ShaderProgram pipeShader("pipe", "obj", mvp, pipeAngle);
  ShaderProgram explodeShader("explode", "obj", mvp, explodeTime);
  ShaderProgram digitShader("digit", "obj", mvp);

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection =
    glm::perspective(glm::radians(45.0f), (float)Width / (float)Height, 0.1f, 100.0f);

  TextureLibrary textureLibrary(r.get());
  std::vector<std::unique_ptr<Obj>> bird;
  for (int i = 0; i < 4; ++i)
    bird.push_back(std::make_unique<Obj>(textureLibrary, "bird" + std::to_string(i + 1), "bird"));
  Obj pipe(textureLibrary, "pipe");
  std::vector<std::unique_ptr<Obj>> digits;
  for (int i = 0; i < 10; ++i)
    digits.push_back(std::make_unique<Obj>(textureLibrary, std::to_string(i)));

  auto currentTime = SDL_GetTicks();
  auto isCollision = false;
  auto countDown = currentTime;
  auto alpha = 0.0f;
  int oldN = 0;
  while (!done)
  {
    while (e.poll()) {}

    // Camera matrix
    glm::mat4 View =
      glm::lookAt(glm::vec3(0, camY, 3),
                  glm::vec3(0, 0, 0), // and looks at the origin
                  glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
      );

    // Our ModelViewProjection : multiplication of our 3 matrices

    if (isCollision)
      glClearColor(1.0f, 0.7f, 7.0f, 0.0f);
    else
      glClearColor(0.5f, 0.7f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!isCollision)
      objShader.use();
    else
    {
      explodeTime += 0.1f;
      explodeShader.use();
      explodeTime.update();
    }
    const auto BirdScreenX = -12.0f;
    glm::mat4 ModelBird = glm::translate(glm::vec3(BirdScreenX, 0, birdY)) *
                          glm::rotate(-4.0f * birdVY, glm::vec3(0.0f, 1.0f, 0.0f)) *
                          glm::rotate(3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
                          glm::mat4(1.0f);
    mvp = Projection * View * ModelBird; // Re
    mvp.update();
    bird[SDL_GetTicks() / 50 % bird.size()]->activate();

    const auto SpacingK = 13;
    {
      digitShader.use();
      int n = (birdX + BirdScreenX + 3.0) / SpacingK + 1;
      if (oldN != n)
        mixer.play(coin);
      oldN = n;
      int pos = 0;
      do
      {
        glm::mat4 ModelDigit = glm::translate(glm::vec3(pos * 1.5f, -2, 6)) *
                               glm::rotate(0.5f * sin(alpha), glm::vec3(0.0f, 0.0f, 1.0f)) *
                               glm::translate(glm::vec3(-0.5, 0, 0)) *
                               glm::rotate(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                               glm::mat4(1.0f);
        mvp = Projection * View * ModelDigit;
        mvp.update();
        digits[n % 10]->activate();
        --pos;
        n /= 10;
      } while (n != 0);
      alpha += 0.03;
    }

    pipeShader.use();
    pipeAngle += 0.01;
    pipeAngle.update();
    for (int x = (birdX + BirdScreenX) / SpacingK; x <= (birdX - BirdScreenX + SpacingK) / SpacingK;
         ++x)
    {
      if (x < 0 || x >= static_cast<int>(pipes.size()))
        continue;

      glm::mat4 ModelBottomPipe = glm::translate(glm::vec3(x * SpacingK - birdX, 0, pipes[x] - 3)) *
                                  glm::rotate(3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                  glm::mat4(1.0f);

      mvp = Projection * View * ModelBottomPipe; // Re
      mvp.update();
      pipe.activate();

      glm::mat4 ModelTopPipe = glm::translate(glm::vec3(x * SpacingK - birdX, 0, pipes[x] + 3)) *
                               glm::rotate(-3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
                               glm::mat4(1.0f);
      mvp = Projection * View * ModelTopPipe; // Re
      mvp.update();
      pipe.activate();
    }

    r.present();
    auto time = SDL_GetTicks();
    while (time > currentTime)
    {
      currentTime += 10;
      if (!gameIsStarted)
        continue;

      if (!isCollision)
      {
        birdX += 0.1;
        birdY += birdVY;
        birdVY -= 0.003f;
        float index;
        float fraction = modf((birdX + BirdScreenX + 3.0) / SpacingK, &index);
        isCollision = (index >= 0 && fraction >= 0 && fraction < 0.5f &&
                       (birdY < pipes[index] - 2 || birdY > pipes[index] + 2)) ||
                      (birdY < -10);
        if (isCollision)
        {
          mixer.play(explosion);
          countDown = currentTime + 2000;
          explodeTime = 1;
        }
      }

      if (isCollision && (currentTime > countDown))
      {
        birdY = 0;
        birdVY = 0;
        birdX = 0;
        gameIsStarted = false;
        isCollision = false;
      }
    }
  }
}
catch (std::exception &e)
{
  std::cout << e.what() << std::endl;
}
