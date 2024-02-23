
#ifndef _SHADER_H_
#define _SHADER_H_

#include "types.h"

typedef struct {
  void* handle; // standin for FILE*
  const char* filename; // remove?
  char* text;
  int length;
} File;

void file_new(File* file);
void file_open_async(File* file, const char* filename);
uint file_get_length(File* file);
uint file_read(File* file);
void file_delete(File* file);

typedef struct {
  int   handle;
  void* file; // FILE*
  const char* filename;
  char* text;
  int   len;
  int   ready;
} Shader;

void shader_new(Shader* s, const char* filename);
int  shader_load(Shader* s);
int  shader_build(Shader* s);
void shader_delete(Shader* s);

typedef struct {
  int handle;
  int ready;
} ShaderProgram;

void shader_program_new(ShaderProgram* sprog);
int  shader_program_build(ShaderProgram* p, Shader* vert, Shader* frag);
void shader_program_use(ShaderProgram* p);

#endif
