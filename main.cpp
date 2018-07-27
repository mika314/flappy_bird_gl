#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

#include "array_buffer.hpp"
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
  sdl::Window w("Go Cube", 10, 35, Width, Height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
  sdl::Renderer r(w.get(), -1, SDL_RENDERER_PRESENTVSYNC);
  sdl::EventHandler e;
  bool done = false;
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };

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

    -1.0f, 1.0f,  1.0f,  //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, 1.0f,  //

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

    1.0f,  1.0f,  1.0f,  //
    -1.0f, 1.0f,  1.0f,  //
    1.0f,  -1.0f, 1.0f   //
  };

  ArrayBuffer vertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data), 3, 0);
  vertexBuffer.activate();

  // One color for each vertex. They were generated randomly.
  static const GLfloat g_color_buffer_data[] = {
    0.583f, 0.771f, 0.014f, //
    0.609f, 0.115f, 0.436f, //
    0.327f, 0.483f, 0.844f, //
    0.822f, 0.569f, 0.201f, //
    0.435f, 0.602f, 0.223f, //
    0.310f, 0.747f, 0.185f, //
    0.597f, 0.770f, 0.761f, //
    0.559f, 0.436f, 0.730f, //
    0.359f, 0.583f, 0.152f, //
    0.483f, 0.596f, 0.789f, //
    0.559f, 0.861f, 0.639f, //
    0.195f, 0.548f, 0.859f, //
    0.014f, 0.184f, 0.576f, //
    0.771f, 0.328f, 0.970f, //
    0.406f, 0.615f, 0.116f, //
    0.676f, 0.977f, 0.133f, //
    0.971f, 0.572f, 0.833f, //
    0.140f, 0.616f, 0.489f, //
    0.997f, 0.513f, 0.064f, //
    0.945f, 0.719f, 0.592f, //
    0.543f, 0.021f, 0.978f, //
    0.279f, 0.317f, 0.505f, //
    0.167f, 0.620f, 0.077f, //
    0.347f, 0.857f, 0.137f, //
    0.055f, 0.953f, 0.042f, //
    0.714f, 0.505f, 0.345f, //
    0.783f, 0.290f, 0.734f, //
    0.722f, 0.645f, 0.174f, //
    0.302f, 0.455f, 0.848f, //
    0.225f, 0.587f, 0.040f, //
    0.517f, 0.713f, 0.338f, //
    0.053f, 0.959f, 0.120f, //
    0.393f, 0.621f, 0.362f, //
    0.673f, 0.211f, 0.457f, //
    0.820f, 0.883f, 0.371f, //
    0.982f, 0.099f, 0.879f  //
  };
  ArrayBuffer colorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data), 3, 1);
  colorBuffer.activate();

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
  ArrayBuffer uvBuffer(g_uv_buffer_data, sizeof(g_uv_buffer_data), 2, 2);
  uvBuffer.activate();

  // Load texture
  sdl::Texture texture(r.get(), sdl::Surface("texture.bmp").get());
  texture.glBind(nullptr, nullptr);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  const auto VertexFile = "simple_vertex_shader.vertexshader";
  const auto FragmentFile = "simple_fragment_shader.fragmentshader";

  Var<float> alpha("alpha", 0);
  Var<int> x("x");
  Var<int> y("y");
  Var<glm::mat4> mvp("mvp");

  ShaderProgram shader(VertexFile, FragmentFile, alpha, x, y, mvp);
  while (!done)
  {
    while (e.poll()) {}

    shader.use();
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection =
      glm::perspective(glm::radians(45.0f), (float)Width / (float)Height, 0.1f, 100.0f);

    // Or, for an ortho camera :
    // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world
    // coordinates

    // Camera matrix
    glm::mat4 View =
      glm::lookAt(glm::vec3(4.5, 10, 4.5),
                  glm::vec3(4.5, 4.5, 0), // and looks at the origin
                  glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
      );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    mvp = Projection * View * Model; // Re

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    alpha.update();
    for (y = 0; y < 3; ++y)
      for (x = 0; x < 3; ++x)
      {
        x.update();
        y.update();
        mvp.update();
        // Draw the triangle !
        glDrawArrays(
          GL_TRIANGLES, 0, 3 * 12); // Starting from vertex 0; 3 vertices total -> 1 triangle
      }
    alpha += 0.002;
    r.present();
  }
}
catch (std::exception &e)
{
  std::cout << e.what() << std::endl;
}
