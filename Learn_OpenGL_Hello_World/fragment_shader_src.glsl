#version 330 core
	in vec3 colour;
	in vec2 texCoord;

	out vec4 FragColor;

	uniform vec4 uniformColour;
	uniform sampler2D imageTexture;

	void main()
	{
		// FragColor = vec4(uniformColour_1, uniformColour_2, uniformColour_3);
		// FragColor = vec4(colour.x + uniformColour.x, colour.y + uniformColour.y, colour.z + uniformColour.z, 1.0f);

		FragColor = texture(imageTexture, texCoord) * vec4(colour.x + uniformColour.x, colour.y + uniformColour.y, colour.z + uniformColour.z, 0.8f);
	}