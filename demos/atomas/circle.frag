#version 450

in vec2 pos;

out vec4 fragColor;

#define border 0.3

void main(){
    if(pos.x * pos.x + pos.y * pos.y <= 1)
        fragColor = vec4(1, 1, 1, 1);
    else 
        fragColor = vec4(0, 0, 0, 1);
}

