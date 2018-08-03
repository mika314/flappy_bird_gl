#include "obj.hpp"
#include <fstream>
#include <sstream>

class ObjData
{
public:
  ObjData(const std::string &fileName)
  {
    std::ifstream obj("data/" + fileName + ".obj");
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
        std::vector<std::tuple<int,int,int>> idxs;
        while (strm)
        {
          std::string tmp;
          std::getline(strm, tmp, '/');
          int v = !tmp.empty() ? std::stoi(tmp) - 1 : -1;
          std::getline(strm, tmp, '/');
          int uv = !tmp.empty() ? std::stoi(tmp) - 1 : -1;
          std::getline(strm, tmp, ' ');
          int normal = !tmp.empty() ? std::stoi(tmp) - 1 : -1;
          if (!strm)
            break;
          idxs.emplace_back(v, uv, normal);
        }
        if (idxs.size() >= 3)
        {
          auto addIdx = [&](int i) {
            auto idx = idxs[i];
            if (std::get<0>(idx) >= 0)
              vertices.push_back(tmpVertices[std::get<0>(idx)]);
            else
              vertices.emplace_back();
            if (std::get<1>(idx) >= 0)
              uvs.push_back(tmpUvs[std::get<1>(idx)]);
            else
              uvs.emplace_back();
            if (std::get<2>(idx) >= 0)
              normals.push_back(tmpNormals[std::get<2>(idx)]);
            else
              normals.emplace_back();
          };

          for (size_t i = 1; i < idxs.size() - 1; ++i)
          {
            addIdx(0);
            addIdx(i);
            addIdx(i + 1);
          }
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

Obj::Obj(TextureLibrary &textureLibrary,
         const std::string &objFileName,
         const std::string &textureFileName)
  : texture(textureLibrary.get(textureFileName.empty() ? objFileName : textureFileName)),
    objData(std::make_unique<ObjData>(objFileName)),
    vertices(objData->vertices.data(), objData->vertices.size(), 0),
    uvs(objData->uvs.data(), objData->uvs.size(), 1),
    normals(objData->normals.data(), objData->normals.size(), 2),
    n(objData->vertices.size())
{
  objData = nullptr;
}

Obj::~Obj() {}

void Obj::activate()
{
  vertices.activate();
  normals.activate();
  if (texture)
  {
    uvs.activate();
    texture->glBind(nullptr, nullptr);
  }
  glDrawArrays(GL_TRIANGLES, 0, n);
}
