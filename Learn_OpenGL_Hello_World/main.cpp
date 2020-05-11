#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	float textureCoordsContainer[] = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};

	float textureCoordsFace[] = {
		
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	unsigned int indices[] = {
		0, 1, 2, // triangle #1
		3, 4, 5, // triangle #2
		0, 2, 3, // triangle #3
		5, 2, 3  // triangle #4
	};

	Shader shaderProgram = Shader("vertex_shader_src.glsl", "fragment_shader_src.glsl");

	// -------------------------------------------------------------------------------------------------------------------------
	// Generate, bind, and fill main Vertex Array Object (VAO) and Vertex Buffer Objects (VBOs)
	unsigned int VAO, VBO_vertices, VBO_colours, VBO_containerTexCoords, VBO_faceTexCoords;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_vertices);
	glGenBuffers(1, &VBO_colours);
	glGenBuffers(1, &VBO_containerTexCoords);
	glGenBuffers(1, &VBO_faceTexCoords);
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
	// Texture VBOs
	// Container texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBO_containerTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsContainer), textureCoordsContainer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Smiling face texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBO_faceTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsFace), textureCoordsFace, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Enable attributes for VAO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	// Create Element Buffer Object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 12, indices, GL_STATIC_DRAW);
	// Bind EBO to VAO as well
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// -------------------------------------------------------------------------------------------------------------------------

	// Texture loading 0
	// Generate an OpenGL texture and add data from the loaded .jpg image
	glActiveTexture(GL_TEXTURE0); // activate the texture unit, then bind the texture
	unsigned int woodenTexture;
	glGenTextures(1, &woodenTexture);
	glBindTexture(GL_TEXTURE_2D, woodenTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // texture magnification doesn't use mipmaps
	// Load texture as bytes (chars)
	int texWidth, texHeight, numChannels;
	unsigned char* containerImageData = stbi_load("wooden_container_texture.jpg", &texWidth, &texHeight, &numChannels, 0);
	if (containerImageData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, containerImageData);
		// OpenGL function to generate a mipmap with the given texture
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR: Failed to load the container texture image" << std::endl;
		return -1;
	}
	stbi_image_free(containerImageData);

	// Texture loading 1
	// Generate an OpenGL texture and add data from the loaded .jpg image
	glActiveTexture(GL_TEXTURE1); // activate the texture unit, then bind the texture
	unsigned int smilingTexture;
	glGenTextures(1, &smilingTexture);
	glBindTexture(GL_TEXTURE_2D, smilingTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // texture magnification doesn't use mipmaps
	// Load texture as bytes (chars)
	int texWidth2, texHeight2, numChannels2;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* smilingImageData = stbi_load("smiling_texture.jpg", &texWidth2, &texHeight2, &numChannels2, 0);
	if (smilingImageData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth2, texHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, smilingImageData);
		// OpenGL function to generate a mipmap with the given texture
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR: Failed to load the smiling texture image" << std::endl;
		return -1;
	}
	stbi_image_free(smilingImageData);

	// Use defined and compiled vertex/fragment shaders
	shaderProgram.use();
	shaderProgram.setInt("imageTexture1", 0); 
	shaderProgram.setInt("imageTexture2", 1); 

	// GLM matric transformation practice
	//
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f); // vec (1, 0, 0)
	// Create 4x4 transformation matrix (translation)
	glm::mat4 translateMatrix(1.0f);
	translateMatrix = glm::translate(translateMatrix, glm::vec3(1.0f, 1.0f, 0.0f));
	// Do the translation to the vector
	vec = translateMatrix * vec; 
	std::cout << "Transformed vector: (" << vec.x << "," << vec.y << "," << vec.z << ") " << std::endl;
	// Debugging
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << translateMatrix[j][i] << ",";
		}
		std::cout << std::endl;
	}

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

		// Colour changing with time
		float timeValue = glfwGetTime();
		float redValue   = (sin(timeValue - 1) / 2.0f) + 0.5f;
		float greenValue = (sin(timeValue    ) / 2.0f) + 0.5f;
		float blueValue  = (sin(timeValue + 1) / 2.0f) + 0.5f;
		shaderProgram.setFloat4("uniformColour", redValue, greenValue, blueValue, 1.0f);

		// Transformation matrix to pass as uniform variable
		glm::mat4 trans(1.0f); // Init identity matrix
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f)); // glm::radians(90.0f)
		//trans = glm::scale(trans, glm::vec3(2.0f, 2.0f, 2.0f));
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		shaderProgram.setMatrix4("transform", trans);

		// Bind predefined Vertex Array Object (indirectly binds its attached VBOs and texture)
		glBindVertexArray(VAO);
		// Draw from Element Buffer Object indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Second matrix transformation
		trans = glm::mat4(1.0f); // reset to identity matrix
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		float scaleVal = sin(glfwGetTime());
		trans = glm::scale(trans, glm::vec3(scaleVal, scaleVal, scaleVal));
		shaderProgram.setMatrix4("transform", trans);
		// Draw again but with different transformation matrix
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		// Check events and swap frame buffers (avoids flickering)
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
	glDeleteBuffers(1, &VBO_containerTexCoords);
	glDeleteBuffers(1, &VBO_faceTexCoords);
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