#version 450

uniform vec3 color;

out vec4 fragColor;

in vec2 position;

void main(){

  float opacity;

  if(length(position) > 1)
    opacity = 0;
  else
    opacity = 1;

  fragColor = vec4(color, opacity);
}
