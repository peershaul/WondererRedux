#version 450

#define BRICK_COUNT 100
#define COLOR_STATES 5

out vec4 fragColor;

in vec3 color;

void main(){
  fragColor = vec4(color, 1);
}
