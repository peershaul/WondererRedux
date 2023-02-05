#version 450 

in vec2 position;
in vec3 color;

out vec4 fragColor;

void main(){
	float opacity = length(position) > 1? 0 : 1;
	fragColor = vec4(color, opacity);
}
