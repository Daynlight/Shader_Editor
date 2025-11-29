#version 430 core

layout(location = 0) in vec2 aPos;
out vec4 vPos;

void main() {
  gl_Position = vec4(aPos, 0.0, 1.0);
  vPos = gl_Position;
}
