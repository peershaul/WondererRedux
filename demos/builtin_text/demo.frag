#version 450 

out vec4 fragColor;

in vec2 coords;

uniform sampler2D font; 
uniform float threshold;

void main(){
	vec3 font_data = texture(font, coords).xyz;
	if(length(font_data) >= threshold)
		fragColor = vec4(1, 1, 1, 1);
	else 
		fragColor = vec4(0, 0, 0, 0);
}
