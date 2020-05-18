#version 330 core
	in vec3 Normal;
	in vec3 FragPos;
	in vec2 TexCoords;

	out vec4 FragColor;

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

		// Implementing attenuation: f_att = 1.0 / (constant + linear*distance + quadratic*distance^2)
		float constant;
		float linear;
		float quadratic;
	};
	uniform Light light;

	struct SpotLight {
		bool on;
		vec3 position;
		vec3 direction;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		float constant;
		float linear;
		float quadratic;

		// Angle of spotlight
		float cutOff;
		float outerCutOff;
	};
	uniform SpotLight flashlight;

void main() {

	// Point light
	//
	// Phong model implementation
	// Includes ambient, diffuse, specular lighting components
	//
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(0.0, dot(norm, lightDir));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// Specular
	float shininess = 128;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * material.specular) * light.specular;
	// Attenuation computation
	float lightDistance = length(light.position - FragPos);
	float attenuation = 2.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Flashlight
	// 
	vec3 fl_ambient = vec3(0.0f);
	vec3 fl_diffuse = vec3(0.0f);
	vec3 fl_specular = vec3(0.0f);
	// Compare light angle to cutOff
	if(flashlight.on) {
		vec3 flashlightDir = normalize(flashlight.position - FragPos);
		// flashlight ambient
		fl_ambient = flashlight.ambient * vec3(texture(material.diffuse, TexCoords));
		// flashlight diffuse 
		float fl_diff = max(0.0, dot(norm, flashlightDir));
		fl_diffuse = flashlight.diffuse * fl_diff * vec3(texture(material.diffuse, TexCoords));
		// flashlight specular 
		float fl_shininess = 16;
		vec3 fl_reflectDir = reflect(-flashlightDir, norm);
		float fl_spec = pow(max(dot(viewDir, fl_reflectDir), 0.0), material.shininess);
		fl_specular = (fl_spec * material.specular) * flashlight.specular;
		// Flashlight attenuation
		float flashlightDistance = length(flashlight.position - FragPos);
		float fl_attenuation = 2.6 / (flashlight.constant + flashlight.linear * flashlightDistance + flashlight.quadratic * (flashlightDistance * flashlightDistance));
		fl_ambient *= fl_attenuation;
		fl_diffuse *= fl_attenuation;
		fl_specular *= fl_attenuation;
		// Smooth flashlight edge transition
		float theta = dot(flashlightDir, normalize(-flashlight.direction));
		float epsilon = flashlight.cutOff - flashlight.outerCutOff;
		float fl_intensity = clamp((theta - flashlight.outerCutOff) / epsilon, 0.0, 1.0);
		fl_ambient *= fl_intensity;
		fl_diffuse *= fl_intensity;
		fl_specular *= fl_intensity;
	}

	// Combine
	vec3 result = ambient  + diffuse + specular + fl_ambient + fl_diffuse + fl_specular;
	// vec3 result =  fl_ambient + fl_diffuse + fl_specular;
	FragColor = vec4(result, 1.0);

}