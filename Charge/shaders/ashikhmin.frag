// Fragment shader for Ashikhmin direct lighting
#version 410

// Definitions if any
const float M_PI = 3.14159265359f;
const int maxLights = 8;

// Inputs
in vec3 position;
in vec3 normal;

// Uniform variables
uniform int numLights;
uniform vec3 camPos;
uniform vec3 lightCols[maxLights];
uniform vec4 lights[maxLights];
uniform samplerCube skybox;

uniform float n_u;
uniform float n_v;
uniform float rs;
uniform float rd;
uniform vec3 diffuse;
uniform vec3 specular;

uniform int useMask;
uniform mat4 model;

// Outputs
out vec4 fragColor;

// Perlin noise hash to generate a "pseudorandom" number
float rand(vec2 co)
{
   return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	vec3 k1 = normalize(camPos - position);
	vec3 output = vec3(0, 0, 0);
	vec3 uVec = normalize(cross(vec3(0, 1, 0), normal));
	if(length(uVec) < 0.0001) 
		uVec = normalize(cross(vec3(1, 0, 0), normal));
	vec3 vVec = normalize(cross(normal, uVec));
	// Do a hashing of the normal to generate consistent pseudorandom numbers
	float zeta1 = rand(normal.xy);
	float zeta2 = rand(normal.zx);
	float phi = atan(sqrt((n_u + 1.0) / (n_v + 1.0)) * tan(M_PI * zeta1 / 2.0));
	int quadrant = int(rand(normal.yz) * 4.0);
	if(quadrant == 1) phi = M_PI - phi;
	else if(quadrant == 2) phi = M_PI + phi;
	else if(quadrant == 3) phi = 2.f * M_PI - phi;
	float cosTheta = pow((1.0 - zeta2), 1.0 / 
		(1.0 + (n_u * pow(cos(phi), 2)) + (n_v * pow(sin(phi), 2))));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	vec3 hVec = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
	hVec = normalize(hVec.y * uVec + hVec.z * normal + hVec.x * vVec);
	// Calculate direct lighting
	for(int i = 0; i < numLights; i++){
		vec4 lightPos = lights[i];
		vec3 lightCol = lightCols[i];
		vec3 k2;
		if(lightPos.w < 0.001){
			k2 = normalize(vec3(lightPos));
		} else{
			k2 = normalize(vec3(lightPos) - position);
		}
		vec3 halfVec = normalize(k1 + k2);
		float fresnel = rs + (1.0 - rs) * pow(1.0 - dot(k1, halfVec), 5);
		float spec = (sqrt((n_u + 1.0) * (n_v + 1.0)) / (8.0)) * fresnel *
			pow(dot(normal, halfVec), ((n_u * pow(dot(halfVec, uVec), 2) + n_v *
			pow(dot(halfVec, vVec), 2)) / (1.0 - pow(dot(halfVec, normal), 2)))) /
			(dot(halfVec, k1) * max(dot(normal, k1), dot(normal, k2)));
		float diff = ((28.0 * rd) / (23.0)) * rd *		(1.0 - pow(1.0 - dot(normal, k1) / 2.f, 5)) *		(1.0 - pow(1.0 - dot(normal, k2) / 2.f, 5));
		// Clamp just in case
		if(diff < 0) diff = 0.0; if(diff > 1) diff = 1.0;
		if(spec < 0) spec = 0.0; if(spec > 1) spec = 1.0;
		output += diff * diffuse * lightCol + spec * specular * lightCol;
	}
	// Now do an estimate of indirect lighting
	vec3 outgoing = reflect(k1, hVec);
	if(dot(outgoing, normal) < 0) 
		outgoing = -outgoing;
	// Do a small shift on the output vector to move it with the object
	outgoing = normalize(vec3(model * vec4(outgoing, 1.0)));
	//output += rs * specular * vec3(texture(skybox, outgoing));
	// Clamp the resulting color just in case
	if(output.x > 1.0) output.x = 1.0;
	if(output.y > 1.0) output.y = 1.0;
	if(output.z > 1.0) output.z = 1.0;
	output.y /= 2.0;
	if(useMask == 1){
		fragColor = vec4(output, 1.0);
	} else{
		fragColor = vec4(output, 0.0);
	}
}