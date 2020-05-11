#version 330 core
	in vec3 colour;
	in vec2 texCoordContainer;
	in vec2 texCoordFace;

	out vec4 FragColor;

	uniform vec4 uniformColour;
	uniform sampler2D imageTexture1;
	uniform sampler2D imageTexture2;

	void main()
	{
		// FragColor = vec4(uniformColour_1, uniformColour_2, uniformColour_3);
		// FragColor = vec4(colour.x + uniformColour.x, colour.y + uniformColour.y, colour.z + uniformColour.z, 1.0f);
		// FragColor = texture(imageTexture2, texCoordFace); // * vec4(colour.x + uniformColour.x, colour.y + uniformColour.y, colour.z + uniformColour.z, 0.8f);

		FragColor = mix( texture(imageTexture1, texCoordContainer), texture(imageTexture2, texCoordFace), 0.5);

	}