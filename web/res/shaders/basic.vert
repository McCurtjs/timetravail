#version 300 es
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 projViewMod;

out lowp vec4 vColor;

void main() {
  gl_Position = projViewMod * position;
  vColor = color;
}
