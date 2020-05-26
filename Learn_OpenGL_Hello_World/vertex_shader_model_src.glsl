#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aNormal;
	layout (location = 2) in vec2 aTexCoords;
	
	out vec2 TexCoords;

	out vec3 Normal;
	out vec3 FragPos;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

void main()
{
	// TODO: costly to invert matrices in shaders, should pass this as a uniform to optimize 
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	Normal = normalMatrix * aNormal; 
    TexCoords = aTexCoords; 
	FragPos = (model * vec4(aPos, 1.0)).xyz;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}
