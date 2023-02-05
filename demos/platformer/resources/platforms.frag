#version 450 

flat in int is_top;
in vec2 position;

out vec4 fragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main(){
	vec2 coords = (position + vec2(1, 1)) / 2;
	fragColor = texture(is_top == 0? tex1 : tex0, coords); 
}
