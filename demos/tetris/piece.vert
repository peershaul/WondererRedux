#version 450

#define PIECE_AMOUNT 200

layout(location=0) in vec2 aPos;

uniform mat4 models[PIECE_AMOUNT];
uniform mat4 cam_matrix;

void main(){
	gl_Position = cam_matrix * models[gl_InstanceID] * vec4(aPos, 0, 1);
}
