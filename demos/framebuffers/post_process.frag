#version 450 

in vec2 tex_coords;
out vec4 fragColor;

uniform sampler2D frame;

void main(){
   vec3 color = texture(frame, tex_coords).xyz;
   float lum = 1 - (color.r + color.g + color.b) / 3.0f;

   fragColor = vec4(lum, lum, lum, 1);
}
