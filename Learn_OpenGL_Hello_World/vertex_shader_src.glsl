#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aVertexColour;
	layout (location = 2) in vec2 aTextureCoordContainer;
	layout (location = 3) in vec2 aTextureCoordFace;

	out vec3 colour;
	out vec2 texCoordContainer;
	out vec2 texCoordFace;

	uniform vec4 uniformColour;
	uniform mat4 transform;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;

	void main()
	{
	   colour = aVertexColour;
	   texCoordContainer = aTextureCoordContainer;
	   texCoordFace = aTextureCoordFace;

	   // Matrix multiplication from right to left
	   gl_Position = proj * view * model * vec4(aPos, 1.0);
	   
	   //gl_Position = transform * vec4(aPos, 1.0f);
	   //gl_Position = vec4(aPos.x + uniformColour.x - 0.5f, aPos.y - uniformColour.y + 0.5f, aPos.z + uniformColour.z, 1.0);
	}