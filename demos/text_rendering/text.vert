#version 450 

#define max_letter_count 32

layout(location = 0) in vec2 aPos;

out vec2 coords;

uniform int letters[max_letter_count]; 
uniform mat4 models[max_letter_count];
uniform mat4 cam_matrix;

void main(){
	int letter = letters[gl_InstanceID];

	bvec2 side = bvec2(aPos.x > 0, aPos.y > 0);
	ivec2 position = ivec2(letter % 16, letter / 16);
	
	coords = vec2(side.x? float((position.x + 1) / 16.0f) : 
						  float(position.x / 16.0f),
				  side.y? 1 - float(position.y / 6.0f) : 
						  1 - float((position.y + 1) / 6.0f));

	gl_Position = cam_matrix * models[gl_InstanceID] * vec4(aPos.x, aPos.y, 0, 1);

}


/*#version 450 

layout(location = 0) in vec2 aPos;

out vec2 coords;

uniform int letter;

void main(){
	bool high_x = aPos.x > 0;	
	bool high_y = aPos.y > 0;
	
	int row = letter % 16;
	int col = letter / 16;

	coords = vec2(high_x? float((row + 1) / 16.0f) : float(row / 16.0f),
								high_y? 1 - float(col / 6.0f) : 1 - float((col + 1) / 6.0f));
	
	gl_Position = vec4(aPos, 0, 1); 
}*/

