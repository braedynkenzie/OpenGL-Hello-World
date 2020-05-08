#version 330 core
	in vec3 colour;
	out vec4 FragColor;
	uniform vec4 uniformColour;
	void main()
	{
	//	FragColor = vec4(uniformColour_1, uniformColour_2, uniformColour_3);
		FragColor = vec4(colour.x + uniformColour.x, colour.y + uniformColour.y, colour.z + uniformColour.z, 1.0f);
	}