#version 430 core

out vec4 FragColor;
in vec4 vPos;

uniform vec3 a;
uniform vec2 rad;

void main() {
  vec2 pos = vPos.xy;
  float eq = (pos.x * pos.x) / (rad.x * rad.x) +
             (pos.y * pos.y) / (rad.y * rad.y);
  if(eq < 1 && eq > 0.5){
    FragColor = vec4(a, 1.0);
  } 
  else
    FragColor = vec4(0, 0, 0, 1.0);
  // FragColor = vec4(rad, 0.0, 1.0);

}
