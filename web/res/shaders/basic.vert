#version 300 es
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

//uniform mat4 modelView;
//uniform mat4 projection;

out lowp vec4 vColor;

void main() {
  gl_Position = position;// projection * modelView * position;
  vColor = color;
}
