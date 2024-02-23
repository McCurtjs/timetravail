#include "shader.h"

#include <string.h>

#include "GL/gl.h"
#include "wasm.h"

int hasext(const char* str, const char* ext) {
  const char* strext = strrchr(str, '.');
  return strcmp(strext + 1, ext) == 0;
}

int shader_build(Shader* s, int type, const char* buffer, uint buf_len) {
  s->handle = glCreateShader(type);
  glShaderSource(s->handle, 1, (const char**)&buffer, (const int*)&buf_len);
  glCompileShader(s->handle);

  glGetShaderiv(s->handle, GL_COMPILE_STATUS, &s->ready);
  return s->ready;
}

int shader_build_from_file(Shader* shader, File* file) {
  GLenum type = 0;
  if (hasext(file->filename, "vert")) type = GL_VERTEX_SHADER;
  else if (hasext(file->filename, "frag")) type = GL_FRAGMENT_SHADER;
  file_read(file);
  return shader_build(shader, type, file->text, file->length);
}

void shader_delete(Shader* shader) {
  glDeleteShader(shader->handle);
  shader->handle = 0;
}

void shader_program_new(ShaderProgram* program) {
  program->ready = 0;
}

int shader_program_build(ShaderProgram* p, Shader* vert, Shader* frag) {
  p->handle = glCreateProgram();
  glAttachShader(p->handle, vert->handle);
  glAttachShader(p->handle, frag->handle);
  glLinkProgram(p->handle);

  glGetProgramiv(p->handle, GL_LINK_STATUS, &p->ready);
  return p->ready;
}

void shader_program_use(ShaderProgram* program) {
  glUseProgram(program->handle);
}

void shader_program_delete(ShaderProgram* program) {
  glDeleteProgram(program->handle);
}
