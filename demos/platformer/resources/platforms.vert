#version 450 

#define MAX_PLATFORMS_AMOUNT 100 

layout(location = 0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 model_matrices[MAX_PLATFORMS_AMOUNT];
uniform int is_tops[MAX_PLATFORMS_AMOUNT];

flat out int is_top;
out vec2 position;

void main(){
	is_top = is_tops[gl_InstanceID] == 0? 0 : 1;
	gl_Position = cam_matrix * model_matrices[gl_InstanceID] * vec4(aPos, 0, 1);
	position = aPos;
}
