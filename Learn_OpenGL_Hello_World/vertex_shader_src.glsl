#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aVertexColour;
	layout (location = 2) in vec2 aTextureCoord;

	out vec3 colour;
	out vec2 texCoord;

	uniform vec4 uniformColour;

	void main()
	{
	   colour = aVertexColour;
	   texCoord = aTextureCoord;
	   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);

	   // gl_Position = vec4(aPos.x + uniformColour.x - 0.5f, aPos.y - uniformColour.y + 0.5f, aPos.z + uniformColour.z, 1.0);
	}