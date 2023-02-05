#version 450

layout(location = 0) in vec2 aPos;

out vec2 coords;

void main(){
	coords = vec2(aPos.x < 0? 0 : 1, aPos.y < 0? 0 : 1);
	gl_Position = vec4(aPos, 0, 1);
}
