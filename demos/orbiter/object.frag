#version 450 

#define THRESHOLD 0.75
#define THRESH_OFFSET 0.5

in vec2 position;
in vec3 color;

out vec4 fragColor;

void main(){
	float radius = length(position); 
	float opacity = radius > 1? 0 : 1;
	vec3 final_color = color;
	
	if(radius > 1 - THRESHOLD)
		final_color = mix(color, vec3(1, 1, 1), max(((radius - 1 + THRESHOLD) / THRESHOLD) - THRESH_OFFSET, 0));

	fragColor = vec4(final_color, opacity);
}
