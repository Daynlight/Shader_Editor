#version 430 core

out vec4 FragColor;
in vec4 vPos;

void main() {
  FragColor = vec4(vPos.xyz, 1.0); 
}
