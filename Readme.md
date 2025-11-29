<div align=center>
  <h1>ShaderEditor</h1>
</div>

## Installation

```bash
  git clone --recursive https://github.com/daynlight/Shader_Editor.git
  cd Shader_Editor

  mkdir -p build
  cd build
  cmake ..
  cmake --build . --config Release
  cd ..

  sudo cmake --install build/ --config Release
  cd ..
```



## Usage
1. Go to folder with your project
2. You need to have ```shader.vert```, ```shader.frag```
3. Optionally ```uniforms.txt```, ```texture.png```
4. Open terminal and navigate to this directory.
5. Run ```ShaderEditor```.
6. Now you can edit files and it will be render in window.



## Examples
[Elipse](Example/Elipse/)
[Texture](Example/Texture/)

### Vertex Shader
```glsl
#version 430 core

layout(location = 0) in vec2 aPos;
out vec4 vPos;

void main() {
  gl_Position = vec4(aPos, 0.0, 1.0);
  vPos = gl_Position;
}
```

### Fragment Shader
```glsl
#version 430 core

out vec4 FragColor;
in vec4 vPos;

uniform vec2 rad;

void main() {
  vec2 pos = vPos.xy;
  float eq = (pos.x * pos.x) / (rad.x * rad.x) +
             (pos.y * pos.y) / (rad.y * rad.y);
  if(eq < 1 && eq > 0.5){
    FragColor = vec4(1, 1, 1, 1.0);
  } 
  else
    FragColor = vec4(0, 0, 0, 1.0);
}
```

### Uniforms
```txt
vec2 rad 0.7, 0.2
```



## Allowed Uniforms Types
* int
* ivec2 (int)
* ivec3 (int)
* float
* vec2 (float)
* vec3 (float)



## Planed
- [ ] More textures at once
- [ ] Env Uniforms
- [ ] Icon
- [ ] More Shaders Examples