#include "shader.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "wasm.h"

void file_new(File* file) {
  file->handle = NULL;
  file->filename = NULL;
  file->text = NULL;
  file->length = 0;
}

void file_open_async(File* file, const char* filename) {
  file_new(file);
  file->filename = filename;
  file->handle = fopen(filename, "r");
}

uint file_get_length(File* file) {
  fseek(file->handle, 0, SEEK_END);
  return ftell(file->handle);
}

uint file_read(File* file) {
  if (file->text) return 0;
  file->length = file_get_length(file);
  if (file->length == 0) return 0;
  file->text = (char*)malloc(file->length);
  fseek(file->handle, 0, SEEK_SET);
  fread(file->text, sizeof(char), file->length, file->handle);
  fclose(file->handle);
  file->handle = NULL;
  return file->length;
}

void file_delete(File* file) {
  if (file->handle) fclose(file->handle);
  if (file->text) free(file->text);
  file_new(file);
}

int hasext(const char* str, const char* ext) {
  const char* strext = strrchr(str, '.');
  return strcmp(strext + 1, ext) == 0;
}

void shader_new(Shader* s, const char* filename) {
  s->ready = 0;
  s->filename = filename;
  s->file = fopen(filename, "r");
}

int shader_load(Shader* s) {
  fseek(s->file, 0, SEEK_END);
  s->len = ftell(s->file);
  s->text = (char*)malloc(s->len);
  fseek(s->file, 0, SEEK_SET);
  fread(s->text, sizeof(char), s->len, s->file);
  fclose(s->file);
  s->file = NULL;

  return 1;
}

int shader_build(Shader* s) {
  GLenum type = 0;
  if (hasext(s->filename, "vert")) type = GL_VERTEX_SHADER;
  else if (hasext(s->filename, "frag")) type = GL_FRAGMENT_SHADER;
  s->handle = glCreateShader(type);
  glShaderSource(s->handle, 1, (const char**)&s->text, (const int*)&s->len);
  glCompileShader(s->handle);

  glGetShaderiv(s->handle, GL_COMPILE_STATUS, &s->ready);
  return s->ready;
}

void shader_delete(Shader* shader) {
  glDeleteShader(shader->handle);
  shader->handle = 0;
}

void shader_program_new(ShaderProgram* sprog) {
  sprog->ready = 0;
}

int shader_program_build(ShaderProgram* p, Shader* vert, Shader* frag) {
  p->handle = glCreateProgram();
  glAttachShader(p->handle, vert->handle);
  glAttachShader(p->handle, frag->handle);
  glLinkProgram(p->handle);

  glGetProgramiv(p->handle, GL_LINK_STATUS, &p->ready);
  return p->ready;
}

void shader_program_use(ShaderProgram* p) {
  glUseProgram(p->handle);
}
