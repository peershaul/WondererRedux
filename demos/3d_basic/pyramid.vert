#version 450 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 trans_matrix;

out vec3 color;

void main(){
	gl_Position = trans_matrix * vec4(aPos, 1);
	color = aColor;
}
