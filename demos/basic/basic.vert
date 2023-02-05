#version 450

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aCoords;

out vec3 color;
out vec2 coords;

uniform mat4 cam_mat;

void main(){
  color = aColor;
  coords = aCoords;


  gl_Position = cam_mat * vec4(aPos, 1, 1);
}
