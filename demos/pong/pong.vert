#version 450

layout (location = 0) in vec2 aPos;

uniform mat4 trans_mat;

void main(){
  gl_Position = trans_mat * vec4(aPos, 0, 1);
}
