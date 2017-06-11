// Shader for the UI
#version 330 core

in vec2 TexCoords;
uniform sampler2D tex;

out vec4 fragColor;

void main(){
	fragColor = texture(tex, TexCoords);
}