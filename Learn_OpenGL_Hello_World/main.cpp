#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// define other functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Global variables
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	// Setup version (using OpenGL v3.3 in core-profile mode)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create an OpenGL window using GLFW library implementation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Init GLAD (manages OpenGL function pointers)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Give OpenGL dimensions of window
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Callback function for window being resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// vertices of triangle in NDCS 
	float vertices_old[] = {
		// first triangle
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, 0.5f, 0.0f, // top left
		// second triangle
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f // top left
	};

	float vertices[] = {
		0.5f, 0.5f, 0.0f, // top right 1
		0.5f, -0.5f, 0.0f, // bottom right 1
		0.0f, -0.5f, 0.0f, // bottom left 1
		
		0.0f, 0.5f, 0.0f, // top right 2
		-0.5f, 0.5f, 0.0f, // top left 2
		-0.5f, -0.5f, 0.0f // bottom left 2
	};

	float colours[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	float textureCoords[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
	};

	unsigned int indices[] = {
		0, 1, 2, // triangle #1
		3, 4, 5 //  triangle #2
	};

	Shader shaderProgram = Shader("vertex_shader_src.glsl", "fragment_shader_src.glsl");

	// -------------------------------------------------------------------------------------------------------------------------
	// Generate, bind, and fill main Vertex Array Object (VAO) and Vertex Buffer Objects (VBOs)
	unsigned int VAO, VBO_vertices, VBO_colours, VBO_textureCoords;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_vertices);
	glGenBuffers(1, &VBO_colours);
	glGenBuffers(1, &VBO_textureCoords);
	// Order: Bind the VAO first, then bind and set vertex buffers, and then configure vertex attributes
	glBindVertexArray(VAO);
	// Vertices VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Colours VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Texture VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_textureCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Enable attributes for VAO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	// Create Element Buffer Object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Bind EBO to VAO as well
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// -------------------------------------------------------------------------------------------------------------------------

	// Texture loading
	// Generate an OpenGL texture and add data from the loaded .jpg image
	unsigned int woodenTexture;
	glGenTextures(1, &woodenTexture);
	glBindTexture(GL_TEXTURE_2D, woodenTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // texture magnification doesn't use mipmaps
	// Load texture as bytes (chars)
	int texWidth, texHeight, numChannels;
	unsigned char* imageData = stbi_load("wooden_container_texture.jpg", &texWidth, &texHeight, &numChannels, 0);
	if (imageData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		// OpenGL function to generate a mipmap with the given texture
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR: Failed to load the texture image" << std::endl;
		return -1;
	}
	stbi_image_free(imageData);

	



	// Display graphics loop
	while (!glfwWindowShouldClose(window))
	{
		// user key input processing
		processInput(window);

		// rendering commands section below
		// 
		// Clear colour
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use defined and compiled vertex/fragment shaders
		shaderProgram.use();
		// Bind predefined Vertex Array Object (indirectly binds its attached VBOs and texture)
		glBindVertexArray(VAO);
		// Draw from Element Buffer Object
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Colour changing with time
		float timeValue = glfwGetTime();
		float redValue   = (sin(timeValue - 1) / 2.0f) + 0.5f;
		float greenValue = (sin(timeValue    ) / 2.0f) + 0.5f;
		float blueValue  = (sin(timeValue + 1) / 2.0f) + 0.5f;
		shaderProgram.setFloat4("uniformColour", redValue, greenValue, blueValue, 1.0f);

		// check events and swap frame buffers (avoids flickering)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int numAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
	std::cout << "Maximum number of vertex attributes supported: " << numAttributes << std::endl;

	// Release GLFW resources before exiting
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_vertices);
	glDeleteBuffers(1, &VBO_colours);
	glDeleteBuffers(1, &VBO_textureCoords);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	// Escape key to exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Toggle between modes
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}