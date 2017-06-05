// Basic texture shader - colors with either a texture or solid color
#version 330 core
// Inputs
in vec2 texCoords;

// Uniform variables
uniform int isTexturing;
uniform sampler2D sampler;
uniform vec3 color;
uniform int useMask;

// Outputs
out vec4 fragColor;

void main(){
	if(isTexturing > 0){
		fragColor = texture(sampler, texCoords);
	}
	else{
		fragColor = vec4(color, 1.0);
	}
	if(useMask != 1){
		fragColor.w = 0.0;
	}
	fragColor.y /= 2.0;
}