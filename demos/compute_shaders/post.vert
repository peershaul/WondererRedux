#version 450 

layout(location = 0) in vec2 aPos;

out vec2 tex_coords;

void main(){
   tex_coords = (aPos + vec2(1, 1)) / 2.0f;
   gl_Position = vec4(aPos, 0, 1);
}
