#version 330 core
	in vec3 Normal;
	in vec3 FragPos;

	out vec4 FragColor;

	uniform vec3 objectColor;
	uniform vec3 lightColor;
	uniform vec3 lightPos;
	uniform vec3 viewPos;

void main() {

	// Phong model implementation
	// Includes ambient, diffuse, specular lighting components
	//
	// Ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(0.0, dot(norm, lightDir));
	vec3 diffuse = diff * lightColor;
	// Specular
	float specStrength = 0.8;
	float shininess = 128;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specStrength * spec * lightColor;
	// Combine
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);


}