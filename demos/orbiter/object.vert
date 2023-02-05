#version 450 
#define MAX_ELEMENTS 128

layout(location=0) in vec2 aPos;

out vec2 position;
out vec3 color;

uniform mat4 cam_matrix;
uniform mat4 matrices[MAX_ELEMENTS];
uniform vec3 colors[MAX_ELEMENTS];

void main(){
	position = aPos;
	color = colors[gl_InstanceID];
	gl_Position = cam_matrix * matrices[gl_InstanceID] * vec4(aPos, 0, 1);
}
