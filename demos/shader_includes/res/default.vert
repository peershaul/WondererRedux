#version 450 core

#include "demos/shader_includes/res/get_coords.glsl" 

layout(location = 0) in vec2 aPos;

void main(){
	gl_Position = get_position(aPos);//vec4(aPos * 0.5, 0, 1);	
}

 
