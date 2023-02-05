#version 450

layout(location = 0) in vec2 aPos;

uniform mat4 trans_matrix;

out vec2 position;

void main(){
  gl_Position = trans_matrix * vec4(aPos, 0, 1);
  position = aPos;
}

