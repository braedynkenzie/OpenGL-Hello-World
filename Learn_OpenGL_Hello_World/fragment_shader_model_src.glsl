#version 330 core

	in vec3 Normal;
	in vec2 TexCoords;
	in vec3 FragPos;

	out vec4 FragColor;

	uniform vec3 viewPos;

	uniform sampler2D texture_diffuse1;

	struct Material {
		// Ambient not necessary when using a diffuse map
		// vec3 ambient;
		// material.diffuse is texture_diffuse1 (uniform from model loading)
		sampler2D diffuse;
		vec3 specular;
		float shininess;
	};
	uniform Material material;

	struct PointLight {
		vec3 position;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		// Implementing attenuation: f_att = 1.0 / (constant + linear*distance + quadratic*distance^2)
		float constant;
		float linear;
		float quadratic;
	};
	#define NUM_POINT_LIGHTS 1
	uniform PointLight pointLights[NUM_POINT_LIGHTS];

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

	struct DirLight {
		vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};
	#define NUM_DIR_LIGHTS 1
	uniform DirLight dirLights[NUM_DIR_LIGHTS];

	// Function prototypes
	vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);
	vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
	vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// Set colour as basic diffuse map
    //FragColor = texture(texture_diffuse1, TexCoords);

	// Phong lighting (using directional, point lights, spotlights)
	//
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0f);

	// Directional lighting
	for(int i = 0; i < NUM_DIR_LIGHTS; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir);

	// Point lights
	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

	// Spot light (flashlight)
	result += CalcSpotLight(flashlight, norm, FragPos, viewDir);

	// Combine
	//vec3 result = ambient  + diffuse + specular + fl_ambient + fl_diffuse + fl_specular
	FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// Ambient
	vec3 ambient = pointLight.ambient * vec3(texture(texture_diffuse1, TexCoords));
	// Diffuse 
	vec3 lightDir = normalize(pointLight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = pointLight.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
	// Specular 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = pointLight.specular * spec * material.specular;
	// Attenuation
	float distance = length(pointLight.position - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	// Combine 
	return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	// Ambient 
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
	// Diffuse 
	vec3 lightDir = normalize(-light.direction);
	float diff = max(0.0, dot(normal, lightDir));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular; // modified this line from tutorial
	// Combine 
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 norm, vec3 fragPos, vec3 viewDir)
{
	// Flashlight
	// 
	vec3 fl_ambient = vec3(0.0f);
	vec3 fl_diffuse = vec3(0.0f);
	vec3 fl_specular = vec3(0.0f);
	// Compare light angle to cutOff
	if(flashlight.on) {
		vec3 flashlightDir = normalize(flashlight.position - FragPos);
		// flashlight ambient
		fl_ambient = flashlight.ambient * vec3(texture(texture_diffuse1, TexCoords));
		// flashlight diffuse 
		float fl_diff = max(0.0, dot(norm, flashlightDir));
		fl_diffuse = flashlight.diffuse * fl_diff * vec3(texture(texture_diffuse1, TexCoords));
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
	return (fl_ambient + fl_diffuse + fl_specular);
}