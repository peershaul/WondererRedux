#version 450

in vec2 pos;
in flat int value;

out vec4 fragColor;

#define border 0.3

vec3 value_colors[] = {
    vec3(1, 1, 1),
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1) 
}; 

void main(){
    if(pos.x * pos.x + pos.y * pos.y <= 1)
        fragColor = vec4(value_colors[value], 1);
    else 
        fragColor = vec4(0, 0, 0, 1);
}

