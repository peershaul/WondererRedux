#version 450 

layout(location = 0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 model;

void main(){
    gl_Position = cam_matrix * model * vec4(aPos * 0.5, 0, 1);
}

