#version 330 core
	in vec3 Normal;
	in vec3 FragPos;
	in vec2 TexCoords;

	out vec4 FragColor;

	uniform vec3 lightPos;
	uniform vec3 viewPos;

	struct Material {
		// Ambient not necessary when using a diffuse map
		// vec3 ambient;
		// Replace vec3 diffuse with sampler2D type
		// vec3 diffuse;
		sampler2D diffuse;
		vec3 specular;
		float shininess;
	};
	uniform Material material;

	struct Light {
		vec3 position;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};
	uniform Light light;

void main() {

	// Phong model implementation
	// Includes ambient, diffuse, specular lighting components
	//
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(0.0, dot(norm, lightDir));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// Specular
	float shininess = 128;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * material.specular) * light.specular;
	// Combine
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}