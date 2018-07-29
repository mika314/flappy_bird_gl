#include "shader_program.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static std::string loadFile(const std::string &filename)
{
  std::ifstream strm(filename, std::ios::in);
  if (!strm.is_open())
  {
    std::ostringstream err;
    err << "Impossible to open " << filename << ". Are you in the right directory?\n";
    throw LoadFileError(err.str());
  }
  std::stringstream sstr;
  sstr << strm.rdbuf();
  return sstr.str();
}

static GLuint loadShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string vertexShaderCode = loadFile(vertexFilePath);

  // Read the Fragment Shader code from the file
  std::string fragmentShaderCode = loadFile(fragmentFilePath);

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  std::cout << "Compiling shader: " << vertexFilePath << "\n";
  const char *VertexSourcePointer = vertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::string vertexShaderErrorMessage(InfoLogLength + 1, '\0');
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &vertexShaderErrorMessage[0]);
    throw CompileShaderError(vertexShaderErrorMessage);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragmentFilePath);
  char const *FragmentSourcePointer = fragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::string fragmentShaderErrorMessage(InfoLogLength + 1, '\0');
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &fragmentShaderErrorMessage[0]);
    throw CompileShaderError(fragmentShaderErrorMessage);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    std::string programErrorMessage(InfoLogLength + 1, '\0');
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &programErrorMessage[0]);
    throw CompileShaderError(programErrorMessage);
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

static time_t getModificationTime(const std::string &file)
{
  struct stat statbuf;
  int res = stat(file.c_str(), &statbuf);
  if (res != 0)
    return 0;
  return statbuf.st_mtime;
}

ShaderProgram::ShaderProgram(const std::string &vertex, const std::string &fragment)
  : programId(loadShaders(vertex.c_str(), fragment.c_str())),
    vertexFileName(vertex),
    fragmentFileName(fragment),
    tsVertex(getModificationTime(vertex)),
    tsFragment(getModificationTime(fragment))
{
  glUseProgram(programId);
}

ShaderProgram::~ShaderProgram()
{
  if (programId == 0xffffffffU)
    return;
  glDeleteProgram(programId);
}

void ShaderProgram::use()
{
  auto tsVertexNew = getModificationTime(vertexFileName);
  auto tsFragmentNew = getModificationTime(fragmentFileName);
  if (tsVertexNew > tsVertex || tsFragmentNew > tsFragment)
  {
    tsVertex = tsVertexNew;
    tsFragment = tsFragmentNew;
    try
    {
      auto newProgramId = loadShaders(vertexFileName.c_str(), fragmentFileName.c_str());
      glDeleteProgram(programId);
      programId = newProgramId;
      glUseProgram(programId);
      updateVars();
    }
    catch (CompileShaderError &e)
    {
      std::cerr << e.what();
    }
  }
  glUseProgram(programId);
}

void ShaderProgram::updateVars()
{
  for (auto &&v : vars)
    v->updateLocation(programId);
}
