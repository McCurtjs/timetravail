#ifndef __gl_h_
#ifndef __GL_H__

#define __gl_h_
#define __GL_H__

typedef char GLboolean;
typedef char GLbyte;
typedef char GLchar;
typedef unsigned char GLubyte;
typedef short GLshort;
typedef unsigned short GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned int GLsizei;
typedef unsigned int GLbitfield;
typedef unsigned int GLsizeiptr;
typedef long GLint64;
typedef unsigned long GLuint64;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;

// gl.THING.toString(16)
#define GL_TRUE             1
#define GL_FALSE            0
#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_POINTS           0x0000
#define GL_LINES            0x0001
#define GL_LINE_LOOP        0x0002
#define GL_LINE_STRIP       0x0003
#define GL_TRIANGLES        0x0004
#define GL_TRIANGLE_STRIP   0x0005
#define GL_TRIANGLE_FAN     0x0006
#define GL_LEQUAL           0x0203
#define GL_DEPTH_TEST       0x0B71
#define GL_INVALID_VALUE    0x0501
#define GL_BYTE             0x1400
#define GL_UNSIGNED_BYTE    0x1401
#define GL_INT              0x1404
#define GL_UNSIGNED_INT     0x1405
#define GL_FLOAT            0x1406
#define GL_FRAGMENT_SHADER  0x8B30
#define GL_VERTEX_SHADER    0x8B31
#define GL_COMPILE_STATUS   0x8B81
#define GL_LINK_STATUS      0x8B82
#define GL_ARRAY_BUFFER     0x8892
#define GL_STATIC_DRAW      0x88e4

GLenum  glGetError();
void    glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

GLuint  glCreateShader(GLenum shaderType);
void    glShaderSource(
          GLuint shader, GLsizei count, const GLchar** str, const GLint *len);
void    glCompileShader(GLuint shader);
void    glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
void    glDeleteShader(GLuint program);
GLuint  glCreateProgram();
void    glAttachShader(GLuint program, GLuint shader);
void    glLinkProgram(GLuint program);
void    glGetProgramiv(GLuint program, GLenum pname, GLint* params);
void    glUseProgram(GLuint program);
void    glUniform4fv(GLint loc, GLsizei count, const GLfloat* value);
void    glUniformMatrix4fv(
          GLint loc, GLsizei count, GLboolean transpose, const GLfloat* value);
void    glDeleteProgram(GLuint program);

void    glGenBuffers(GLsizei n, GLuint* buffers);
void    glBindBuffer(GLenum target, GLuint buffer);
void    glBufferData(GLenum tgt, GLsizeiptr size, const void* src, GLenum use);
void    glDeleteBuffers(GLsizei n, const GLuint* buffers);

void    glGenVertexArrays(GLsizei n, GLuint* arrays);
void    glBindVertexArray(GLuint array);
void    glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
void    glVertexAttribPointer(
          GLuint index, GLint size, GLenum type, GLboolean normalized,
          GLsizei stride, const void* pointer);
void    glEnableVertexAttribArray(	GLuint index);
void    glDisableVertexAttribArray(	GLuint index);
void    glDrawArrays(GLenum mode, GLint first, GLsizei count);

GLint   glGetUniformLocation(GLuint program, const GLchar* name);

#endif
#endif
