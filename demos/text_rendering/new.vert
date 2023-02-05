#version 450 

#define max_letters 64

layout(location = 0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 models[max_letters];
uniform int letters[max_letters];

out vec2 coords;

void main(){
	int letter = letters[gl_InstanceID];
	bvec2 side = bvec2(aPos.x > 0, aPos.y > 0);
	
	int row = letter % 16;
	int col = letter / 16;

	coords = vec2(side.x? float((row + 1) / 16.0f) : 
												float(row / 16.0f),
								side.y? 1 - float(col / 6.0f) : 
												1 - float((col + 1) / 6.0f));
	
	gl_Position = cam_matrix * models[gl_InstanceID] * vec4(aPos, 0, 1);
}
