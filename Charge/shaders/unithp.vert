// Vertex shader for Phong shading
#version 330 core

// Inputs
layout(location = 0) in vec3 pos;

// Uniform variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
}