
#include <string.h>

#include "wasm.h"
#include "shader.h"

#include <stdio.h>

#include "GL/gl.h"

int export(canary) (int a) {
  print("WASM is connected!");
  return a * a;
}

Shader test_vert;
Shader test_frag;
ShaderProgram test_shader;
File test_file;

typedef struct {
  float x;
  float y;
} vec2;

typedef struct {
  union { float x; float r; };
  union { float y; float g; };
  union { float z; float b; };
  union { float w; float a; };
} vec4;

typedef struct {
  union {
    float a[16];
    vec4  v[4];
  };
} mat4;

vec2 positions[4] = {
  {-0.5, 0.5},
  {0.5, 0.5},
  {-0.5, -0.5},
  {0.5, -0.5}
};

vec4 colors[4] = {
  {1.0, 1.0, 1.0, 1.0},
  {1.0, 0.0, 0.0, 1.0},
  {0.0, 1.0, 0.0, 1.0},
  {0.0, 0.0, 1.0, 1.0},
};

GLuint pos_buffer;
GLuint color_buffer;

void export(wasm_preload) () {
  shader_new(&test_vert, "./res/shaders/basic.vert");
  shader_new(&test_frag, "./res/shaders/basic.frag");
  file_open_async(&test_file, "./res/shaders/basic.frag");
}

int export(wasm_load) (int await_count) {
  if (await_count) return 0;

  shader_load(&test_vert);
  shader_load(&test_frag);

  shader_build(&test_vert);
  shader_build(&test_frag);

  shader_program_build(&test_shader, &test_vert, &test_frag);

  glGenBuffers(1, &pos_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec2), positions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec4), colors, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  file_read(&test_file);
  print(test_file.text);
  //file_delete(&test_file);

  return 1;
}

void export(wasm_update) (float dt) {

}

void export(wasm_render) () {
  shader_program_use(&test_shader);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// todo:
// make geneic buffer type for transferring data and using for VAOs
// bind model positions from wasm (using buffers)
// split out generic file_ functionality from shaders
// update aspect ratio on window size change
// add error handling (glShaderiv)
// (window event handler?)
// maek cube
//
