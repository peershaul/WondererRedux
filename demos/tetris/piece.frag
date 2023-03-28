#version 450 

out vec4 fragColor;

void main(){
	vec3 color = vec3(1, 1, 1);
	fragColor = vec4(color, 1);
}
