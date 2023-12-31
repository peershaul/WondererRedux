#version 450  

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

uniform mat4 cam_matrix;
uniform mat4 model;

out vec3 color;

void main(){
   gl_Position = cam_matrix * model * vec4(aPos, 1);
   color = aNorm;
}
