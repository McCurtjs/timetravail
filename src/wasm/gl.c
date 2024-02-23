#include "../GL/gl.h"

extern GLenum glGetError();

extern GLuint glCreateShader(GLuint shaderType);
extern void   js_glShaderSource(GLuint data_id, const char* src, GLint length);
void          glShaderSource(
  GLuint shader, GLsizei count, const GLchar** str, const GLint *length
) {
  js_glShaderSource(shader, *str, *length);
}

extern void   glCompileShader(GLuint data_id);
extern void   glDeleteShader(GLuint data_id);
extern int    js_glGetShaderParameter(GLuint data_id, GLenum pname);
void          glGetShaderiv(GLuint data_id, GLenum pname, GLint* params) {
  *params = js_glGetShaderParameter(data_id, pname);
}
extern GLuint glCreateProgram();
extern void   glAttachShader(GLuint program, GLuint shader);
extern void   glLinkProgram(GLuint program);
extern int    js_glGetProgramParameter(GLuint data_id, GLenum pname);
void          glGetProgramiv(GLuint program, GLenum pname, GLint* params) {
  *params = js_glGetProgramParameter(program, pname);
}
extern void   glUseProgram(GLuint program);
extern void   glDeleteProgram(GLuint program);

extern GLuint js_glCreateBuffer();
void          glGenBuffers(GLsizei n, GLuint* buffers
) {
  for (int i = 0; i < n; ++i) {
    buffers[0] = js_glCreateBuffer();
  }
}

extern void   glBindBuffer(GLenum target, GLuint buffer);
extern void   glBufferData(
                GLenum target, GLsizeiptr size, const void* src, GLenum usage);
extern void   glVertexAttribPointer(
                GLuint index, GLint size, GLenum type, GLboolean normalized,
                GLsizei stride, const void* pointer);
extern void   glEnableVertexAttribArray(GLuint index);
extern void   glDisableVertexAttribArray(GLuint index);
extern void   glDrawArrays(GLenum mode, GLint first, GLsizei count);
