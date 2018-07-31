#include "obj.hpp"
#include <fstream>
#include <sstream>

class ObjData
{
public:
  ObjData(const std::string &fileName)
  {
    std::ifstream obj(fileName + ".obj");
    std::string line;

    std::vector<glm::vec3> tmpVertices;
    std::vector<glm::vec2> tmpUvs;
    std::vector<glm::vec3> tmpNormals;

    while (std::getline(obj, line))
    {
      std::istringstream strm(line);
      std::string type;
      strm >> type;
      if (type == "f") // faces
      {
        for (int i = 0; i < 4; ++i)
        {
          std::string tmp;
          std::getline(strm, tmp, '/');
          vertices.push_back(tmpVertices[std::stoi(tmp) - 1]);
          std::getline(strm, tmp, '/');
          uvs.push_back(tmpUvs[std::stoi(tmp) - 1]);
          std::getline(strm, tmp, i != 3 ? ' ' : '\n');
          normals.push_back(tmpNormals[std::stoi(tmp) - 1]);
        }
      }
      // else if (type == "mtlib")
      // else if (type == "o") // object
      // else if (type == "s")
      // else if (type == "usemtl")
      else if (type == "v") // vertex
      {
        float x, y, z;
        strm >> x >> y >> z;
        tmpVertices.emplace_back(x, y, z);
      }
      else if (type == "vn") // normal
      {
        float x, y, z;
        strm >> x >> y >> z;
        tmpNormals.emplace_back(x, y, z);
      }
      else if (type == "vt") // texture coordinates
      {
        float u, v;
        strm >> u >> v;
        tmpUvs.emplace_back(u, 1.0f - v);
      }
    }
  }
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
};

Obj::Obj(SDL_Renderer*renderer, const std::string &fileName):
  texture(renderer, sdl::Surface(fileName + ".bmp").get()),
  objData(std::make_unique<ObjData>(fileName)),
  vertices(objData->vertices.data(), objData->vertices.size(), 0),
  uvs(objData->uvs.data(), objData->uvs.size(), 1),
  normals(objData->normals.data(), objData->normals.size(), 2),
  n(objData->vertices.size())
{
  objData = nullptr;
}

Obj::~Obj()
{}

void Obj::activate()
{
  vertices.activate();
  uvs.activate();
  normals.activate();
  texture.glBind(nullptr, nullptr);
  glDrawArrays(GL_QUADS, 0, n);
}
