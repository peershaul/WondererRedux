#version 450 
#define MAX_ELEMENTS 200

layout(location=0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 matrices[MAX_ELEMENTS];
uniform vec3 colors[MAX_ELEMENTS];

out vec3 color;

void main(){
	gl_Position = cam_matrix * matrices[gl_InstanceID] * vec4(aPos, 0, 1);
	color = colors[gl_InstanceID];
}
