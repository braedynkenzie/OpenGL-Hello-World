#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 4) in vec2 aTexCoords;
	layout (location = 5) in vec3 aNormal;
	
	out vec2 TexCoords;

	out vec3 Normal;
	out vec3 FragPos;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

void main() {
	TexCoords = aTexCoords;
	// NOTE: costly to invert matrices in shaders, should pass this as a uniform to optimize 
	// TODO the above
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	Normal = normalMatrix * aNormal; 
	FragPos = (model * vec4(aPos, 1.0)).xyz;
	gl_Position = proj * view * model * vec4(aPos, 1.0);
}
