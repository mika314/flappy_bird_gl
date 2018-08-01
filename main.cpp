#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include "array_buffer.hpp"
#include "obj.hpp"
#include "sdlpp.hpp"
#include "shader_program.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

int main() try
{
  sdl::Init init(SDL_INIT_EVERYTHING);
  const auto Width = 1280;
  const auto Height = 720;
  sdl::Window w("Go Cube", 64, 126, Width, Height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);

  sdl::Renderer r(w.get(), -1, SDL_RENDERER_PRESENTVSYNC);
  sdl::EventHandler e;
  bool done = false;
  float camY = 13;
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.mouseWheel = [&camY](const SDL_MouseWheelEvent &e) {
    if (e.y > 0)
      camY *= 1.03;
    else
      camY *= 0.97;
  };

  // An array of 3 vectors which represents 3 vertices
  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, -1.0f, // triangle 1 : begin
    -1.0f, -1.0f, 1.0f,  //
    -1.0f, 1.0f,  1.0f,  // triangle 1 : end

    1.0f,  1.0f,  -1.0f, // triangle 2 : begin
    -1.0f, -1.0f, -1.0f, //
    -1.0f, 1.0f,  -1.0f, // triangle 2 : end

    1.0f,  -1.0f, 1.0f,  //
    -1.0f, -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //

    1.0f,  1.0f,  -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    -1.0f, -1.0f, -1.0f, //

    -1.0f, -1.0f, -1.0f, //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, 1.0f,  -1.0f, //

    1.0f,  -1.0f, 1.0f,  //
    -1.0f, -1.0f, 1.0f,  //
    -1.0f, -1.0f, -1.0f, //

    -1.0f, 1.0f,  1.0f, //
    -1.0f, -1.0f, 1.0f, //
    1.0f,  -1.0f, 1.0f, //

    1.0f,  1.0f,  1.0f,  //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  1.0f,  -1.0f, //

    1.0f,  -1.0f, -1.0f, //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  -1.0f, 1.0f,  //

    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //

    1.0f,  1.0f,  1.0f,  //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  1.0f,  //

    1.0f,  1.0f,  1.0f, //
    -1.0f, 1.0f,  1.0f, //
    1.0f,  -1.0f, 1.0f  //
  };

  ArrayBuffer vertexBuffer(
    g_vertex_buffer_data, sizeof(g_vertex_buffer_data) / sizeof(*g_vertex_buffer_data), 3, 0);

  // Make the array with texture coordinates
  static const GLfloat g_uv_buffer_data[] = {
    0, 0, //
    0, 0, //
    0, 0, //

    1, 1, //
    0, 0, //
    0, 1, //

    0, 0.1, //
    0, 0.1, //
    0, 0.1, //

    1, 1, //
    1, 0, //
    0, 0, //

    0, 0.2, //
    0, 0.2, //
    0, 0.2, //

    0, 0.3, //
    0, 0.3, //
    0, 0.3, //

    0, 0.4, //
    0, 0.4, //
    0, 0.4, //

    0, 0.5, //
    0, 0.5, //
    0, 0.5, //

    0, 0.6, //
    0, 0.6, //
    0, 0.6, //

    0, 0.7, //
    0, 0.7, //
    0, 0.7, //

    0, 0.8, //
    0, 0.8, //
    0, 0.8, //

    0, 0.9, //
    0, 0.9, //
    0, 0.9  //
  };
  ArrayBuffer uvBuffer(
    g_uv_buffer_data, sizeof(g_uv_buffer_data) / sizeof(*g_uv_buffer_data), 2, 2);

  std::vector<GLfloat> sphereData;
  auto R = 100;
  for (auto y = 0; y < R; ++y)
  {
    // x^2 + y^2 = R^2;
    // x = +/-sqrt(R^2 - y^2);
    auto x2 = static_cast<int>(sqrt(R * R - y * y));
    auto x1 = -x2;
    for (auto x = x1; x <= x2; ++x)
    {
      sphereData.push_back(x);
      sphereData.push_back(y);
      sphereData.push_back(x + 1);
      sphereData.push_back(y + 1);
      sphereData.push_back(x + 1);
      sphereData.push_back(y);

      sphereData.push_back(x);
      sphereData.push_back(y);
      sphereData.push_back(x);
      sphereData.push_back(y + 1);
      sphereData.push_back(x + 1);
      sphereData.push_back(y + 1);
    }
  }

  ArrayBuffer sphereBuffer(sphereData.data(), sphereData.size(), 2, 3);

  // Load texture
  sdl::Texture texture(r.get(), sdl::Surface("texture.bmp").get());
  texture.glBind(nullptr, nullptr);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  Var<float> alpha("alpha", 0);
  Var<int> x("x");
  Var<int> y("y");
  Var<glm::mat4> mvp("mvp");

  ShaderProgram cubeShader(
    "simple_vertex_shader.vertexshader", "simple_fragment_shader.fragmentshader", alpha, x, y, mvp);
  ShaderProgram sphereShader("sphere.vertexshader", "sphere.fragmentshader", mvp);
  ShaderProgram objShader("obj.vertexshader", "obj.fragmentshader", mvp, alpha);

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection =
    glm::perspective(glm::radians(45.0f), (float)Width / (float)Height, 0.1f, 100.0f);

  // Or, for an ortho camera :
  // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world
  // coordinates

  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model = glm::mat4(1.0f);

  Obj cube(r.get(), "cube");
  Obj robot(r.get(), "robot");

  while (!done)
  {
    while (e.poll()) {}

    cubeShader.use();
    // Camera matrix
    glm::mat4 View =
      glm::lookAt(glm::vec3(3, camY, 3),
                  glm::vec3(3, 3, 0), // and looks at the origin
                  glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
      );

    // Our ModelViewProjection : multiplication of our 3 matrices
    mvp = Projection * View * Model; // Re
    mvp.update();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    alpha.update();
    vertexBuffer.activate();
    uvBuffer.activate();
    texture.glBind(nullptr, nullptr);
    for (y = 0; y < 3; ++y)
      for (x = 0; x < 3; ++x)
      {
        x.update();
        y.update();
        // Draw the triangle !
        glDrawArrays(
          GL_TRIANGLES, 0, 3 * 12); // Starting from vertex 0; 3 vertices total -> 1 triangle
      }

    sphereShader.use();
    mvp.update();
    sphereBuffer.activate();
    glDrawArrays(GL_TRIANGLES, 0, sphereData.size() / 2);
    alpha += 0.002;

    objShader.use();
    mvp.update();
    alpha.update();
    cube.activate();
    robot.activate();

    r.present();
  }
}
catch (std::exception &e)
{
  std::cout << e.what() << std::endl;
}
