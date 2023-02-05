#version 450

#define BRICK_COUNT 100
#define COLOR_STATES 5

layout (location = 0) in vec2 aPos;

uniform mat4 cam_mat;
uniform mat4 trans_mats[BRICK_COUNT];
uniform vec3 color_states[COLOR_STATES];
uniform int hardness_values[BRICK_COUNT];

out vec3 color;

void main(){
  color = color_states[hardness_values[gl_InstanceID] - 1];
  gl_Position = cam_mat * trans_mats[gl_InstanceID] * vec4(aPos, 0, 1);
}
