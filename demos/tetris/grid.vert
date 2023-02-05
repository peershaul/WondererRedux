#version 450

#define GRID_COUNT 200
#define COLOR_COUNT 

layout(location = 0) in vec2 aPos;

uniform mat4 trans_mats[GRID_COUNT];
uniform int spots[GRID_COUNT]; 
uniform vec3 colors[9];

out vec3 color;

void main(){
  color = colors[spots[gl_InstanceID]];
  gl_Position = trans_mats[gl_InstanceID] * vec4(aPos, 0, 1); 
}
