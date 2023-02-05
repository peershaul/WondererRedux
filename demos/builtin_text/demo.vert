#version 450 

#define max_letters 64

layout(location = 0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 models[max_letters];
uniform int letters[max_letters];
uniform int cols;

out vec2 coords;

void main(){
	int letter = letters[gl_InstanceID];
	bvec2 side = bvec2(aPos.x > 0, aPos.y > 0);

	int row = letter % cols;
	int col = letter / cols;

	int rows = cols / (16 * 6);
	coords = vec2(side.x? float((row + 1) / float(cols)) : float(row / float(cols)),
				  side.y? 1 - float(col / float(rows)) : 
						  1 - float(float(col + 1) / float(row)));
	gl_Position = cam_matrix * models[gl_InstanceID] * vec4(aPos, 0, 1);
}
