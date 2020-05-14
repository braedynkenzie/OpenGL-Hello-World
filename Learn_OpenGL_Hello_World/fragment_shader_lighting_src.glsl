#version 330 core
	out vec4 FragColor;

	uniform vec3 objectColor;
	uniform vec3 lightColor;

void main() {

	//vec3 lightingColour = lightColor * objectColor;
	//FragColor = vec4(lightingColour, 1.0);

	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;
	vec3 result = ambient * objectColor;
	FragColor = vec4(result, 1.0);


}