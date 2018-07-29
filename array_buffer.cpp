#include "array_buffer.hpp"

ArrayBuffer::ArrayBuffer(const GLfloat *data, int size, int aCNum, int aAttribute) : attribute(aAttribute), cNum(aCNum), type(GL_FLOAT)
{
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(*data), data, GL_STATIC_DRAW);
}

ArrayBuffer::ArrayBuffer(const GLint *data, int size, int aCNum, int aAttribute) : attribute(aAttribute), cNum(aCNum), type(GL_INT)
{
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(*data), data, GL_STATIC_DRAW);
}

ArrayBuffer::~ArrayBuffer()
{
  glDeleteBuffers(1, &id);
}

void ArrayBuffer::activate()
{
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glEnableVertexAttribArray(attribute);
  glVertexAttribPointer(
    attribute, // attribute. No particular reason for 1, but must match the layout in the shader.
    cNum,      // size
    type,  // type
    GL_FALSE,  // normalized?
    0,         // stride
    (void *)0  // array buffer offset
  );
}
