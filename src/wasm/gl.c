#include "../GL/gl.h"

#include <string.h>

extern GLenum glGetError();
extern void   glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

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
extern void   glUniformMatrix4fv(
                GLint loc, GLsizei count, GLboolean tpose, const GLfloat* mat);
extern void   glDeleteProgram(GLuint program);

extern GLuint js_glCreateBuffer();
void          glGenBuffers(GLsizei n, GLuint* buffers) {
  for (int i = 0; i < n; ++i) buffers[i] = js_glCreateBuffer();
}
extern void   glBindBuffer(GLenum target, GLuint buffer);
extern void   glBufferData(
                GLenum target, GLsizeiptr size, const void* src, GLenum usage);
extern void   js_glDeleteBuffer(int data_id);
void          glDeleteBuffers(GLsizei n, const GLuint* buffers) {
  for (int i = 0; i < n; ++i) js_glDeleteBuffer(buffers[i]);
}

extern GLuint js_glCreateVertexArray();
void          glGenVertexArrays(GLsizei n, GLuint* arrays) {
  for (int i = 0; i < n; ++i) arrays[i] = js_glCreateVertexArray();
}
extern void   glBindVertexArray(GLuint array);
extern void   js_glDeleteVertexArray(int data_id);
void          glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
  for (int i = 0; i < n; ++i) js_glDeleteVertexArray(arrays[i]);
}
extern void   glVertexAttribPointer(
                GLuint index, GLint size, GLenum type, GLboolean normalized,
                GLsizei stride, const void* pointer);
extern void   glEnableVertexAttribArray(GLuint index);
extern void   glDisableVertexAttribArray(GLuint index);
extern void   glDrawArrays(GLenum mode, GLint first, GLsizei count);

extern GLint  js_glGetUniformLocation(GLuint data_id, const char* nam, int len);
GLint         glGetUniformLocation(GLuint program, const GLchar* name) {
  int length = strlen(name);
  return js_glGetUniformLocation(program, name, length);
}
