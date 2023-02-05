#version 450 

in vec3 color;
in vec2 coords;

out vec4 frag_color;

uniform sampler2D tex;
uniform float color_blend;

void main(){
  vec4 clear_color = vec4(1, 1, 1, 1);

  frag_color = (clear_color * (1 - color_blend) + vec4(color, 1) * color_blend) *
    texture(tex, coords); 
}
