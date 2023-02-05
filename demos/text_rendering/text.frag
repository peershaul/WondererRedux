#version 450

in vec2 coords;
out vec4 fragColor;

uniform sampler2D font;
uniform float threshold;

void main(){
	vec3 color = texture(font, coords).xyz;
	if(length(color) > threshold)
		fragColor = vec4(1, 1, 1, 1);
	else 
		fragColor = vec4(0, 0, 0, 1);
}
