// Shader for drawing a 2D rectangle on top of the UI
#version 330 core

uniform vec3 color;
out vec4 fragPos;

void main(){
	fragPos = vec4(color, 1.0);
}