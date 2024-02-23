
#ifndef _SHADER_H_
#define _SHADER_H_

#include "types.h"
#include "file.h"

#define SHADER_VERTEX   0x8B31
#define SHADER_PIXEL    0x8B30

typedef struct {
  int   handle;
  int   ready;
} Shader;

int  shader_build(Shader* s, int type, const char* buffer, uint buffer_length);
int  shader_build_from_file(Shader* s, File* f);
void shader_delete(Shader* s);

typedef struct {
  int handle;
  int ready;
} ShaderProgram;

void shader_program_new(ShaderProgram* program);
int  shader_program_build(ShaderProgram* program, Shader* vert, Shader* frag);
void shader_program_use(ShaderProgram* program);
void shdaer_program_delete(ShaderProgram* program);

#endif
