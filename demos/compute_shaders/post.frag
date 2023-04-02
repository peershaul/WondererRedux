#version 450 

in vec2 tex_coords;
out vec4 fragColor;

uniform sampler2D tex; 

void main(){
   fragColor = texture(tex, tex_coords);
}
