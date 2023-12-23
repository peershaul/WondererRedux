#version 450 

#define MAX_ATOMS_COUNT 25

layout(location = 0) in vec2 aPos;

uniform mat4 cam_matrix;
uniform mat4 models[MAX_ATOMS_COUNT];
uniform int values[MAX_ATOMS_COUNT];

out vec2 pos;
out flat int value;

void main(){
    pos = aPos;
    value = values[gl_InstanceID];
    gl_Position = cam_matrix * models[gl_InstanceID] * vec4(aPos * 0.5, 0, 1);
}

