#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// define other functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);

// Global variables
const unsigned int SCREEN_WIDTH = 800*1.4;
const unsigned int SCREEN_HEIGHT = 600*1.4;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// Create camera object with starting position
Camera camera(cameraPos);
float lastCursorX = SCREEN_WIDTH/2.0f;
float lastCursorY = SCREEN_HEIGHT/2.0f;
float yaw   = 0.0f;
float pitch = 0.0f;
glm::vec3 direction;
bool firstMouseCapture = true;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


float deltaTime = 0.0f; // Time to render last frame
float lastFrame = 0.0f; // Time of last frame

int main() {
	// Setup version (using OpenGL v3.3 in core-profile mode)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create an OpenGL window using GLFW library implementation
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// Hide and capture mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Callback function for window being resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Callback function for mouse cursor movement
	glfwSetCursorPosCallback(window, mouse_callback);

	// Callback function for scrolling zoom
	glfwSetScrollCallback(window, scroll_callback);


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
		-0.5f, -0.5f, 0.0f, // bottom left 2

		0.5f, 0.5f, 1.0f, // Cube front right
		0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 1.0f, // Cube front left
		-0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,

		0.5f, 0.5f, 0.0f, // Cube right right
		0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 0.0f, // Cube right left
		0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,

		-0.5f, 0.5f, 0.0f, // Cube left right
		-0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 0.0f, // Cube left left
		-0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,

		0.5f, 0.5f, 0.0f, // Cube top right
		0.5f, 0.5f, 1.0f,
		-0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, 0.0f, // Cube top left
		-0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 1.0f,

		0.5f, -0.5f, 0.0f, // Cube bottom right
		0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 0.0f, // Cube bottom left
		-0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 1.0f,
	};

	float surfaceNormals[] = {
		0.0f, 0.0f, -1.0f, // Cube back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 1.0f, // Cube front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Cube right face
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f, // Cube left face
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, // Cube top face
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f, // Cube top face
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	float colours[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
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
		0, 1, 2, // Cube back right
		3, 4, 5, // Cube back left
		0, 2, 3, 
		5, 2, 3,  

		6, 7, 8, // Cube front
		9, 10, 11,

		12, 13, 14, // Cube right
		15, 16, 17,

		18, 19, 20, // Cube left
		21, 22, 23,

		24, 25, 26, // Cube top
		27, 28, 29,

		30, 31, 32, // Cube bottom
		33, 34, 35,
	};

	Shader shaderProgram_original = Shader("vertex_shader_src.glsl", "fragment_shader_src.glsl");
	Shader lightingShaderProgram = Shader("vertex_shader_lighting_src.glsl", "fragment_shader_lighting_src.glsl");
	Shader lampShaderProgram = Shader("vertex_shader_light_src.glsl", "fragment_shader_light_src.glsl");


	// -------------------------------------------------------------------------------------------------------------------------
	// Generate, bind, and fill main Vertex Array Object (VAO) and Vertex Buffer Objects (VBOs)
	unsigned int VAO_cube, VBO_vertices, VBO_normals, VBO_colours, VBO_containerTexCoords, VBO_faceTexCoords;
	glGenVertexArrays(1, &VAO_cube);
	glGenBuffers(1, &VBO_vertices);
	glGenBuffers(1, &VBO_normals);
	glGenBuffers(1, &VBO_colours);
	glGenBuffers(1, &VBO_containerTexCoords);
	glGenBuffers(1, &VBO_faceTexCoords);
	// Order: Bind the VAO first, then bind and set vertex buffers, and then configure vertex attributes
	glBindVertexArray(VAO_cube);
	// Surface normals VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surfaceNormals), surfaceNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
	glEnableVertexAttribArray(0); // Vertices
	glEnableVertexAttribArray(1); // Colours
	glEnableVertexAttribArray(2); // Container texture coords
	glEnableVertexAttribArray(3); // Face texture coords
	glEnableVertexAttribArray(4); // normals
	// Create Element Buffer Object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), indices, GL_STATIC_DRAW);
	// Bind EBO to VAO as well
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Unbinding VAO and buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Second VAO for lighting cube
	unsigned int VAO_light;
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);
	// Bind to VBO containing vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	// Set and enable the vertex attributes pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Unbinding VAO and buffer object
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

	// Set textures in shader
	/*shaderProgram.use();
	shaderProgram.setInt("imageTexture1", 0); 
	shaderProgram.setInt("imageTexture2", 1); */

	// Create copies of the cube at different x,y,z locations
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(-4.8f, -0.1f, -6.0f),
		glm::vec3(-4.4f, 3.0f, -6.0f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	// Enable OpenGL z-buffer depth comparisons
	glEnable(GL_DEPTH_TEST);

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Display graphics loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// user key input processing
		processInput(window);

		// Set clear colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Clear colour and z-buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Colour changing with time
		/*float timeValue = glfwGetTime();
		float redValue   = (sin(timeValue - 1) / 2.0f) + 0.5f;
		float greenValue = (sin(timeValue    ) / 2.0f) + 0.5f;
		float blueValue  = (sin(timeValue + 1) / 2.0f) + 0.5f;
		shaderProgram.setFloat4("uniformColour", redValue, greenValue, blueValue, 1.0f);*/

		// Lamp object rendering
		lampShaderProgram.use();
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 1.0f) / 2.0f + 0.7f;
		lightColor.y = sin(glfwGetTime() * 0.5f) / 2.0f + 0.7f;
		lightColor.z = sin(glfwGetTime() * 0.4f) / 2.0f + 0.7f;
		// Model matrix: Translate and scale the light object
		glm::mat4 model_matrix = glm::mat4(1.0f);
		model_matrix = glm::translate(model_matrix, lightPos);
		model_matrix = glm::scale(model_matrix, glm::vec3(0.2f));
		// View matrix: camera
		glm::mat4 view_matrix = camera.GetViewMatrix();
		// Proj matrix: Zoom/Field of View (FOV), set aspect ratio, front and back clipping of view frustum 
		glm::mat4 projection_matrix(1.0f);
		projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		// Set uniforms in shader program
		// Model, view, projection matrices
		lampShaderProgram.setMatrix4("model", model_matrix);
		lampShaderProgram.setMatrix4("view" , view_matrix);
		lampShaderProgram.setMatrix4("proj" , projection_matrix);
		// Light colour uniform
		lampShaderProgram.setVec3("lightColor", lightColor);
		glBindVertexArray(VAO_light);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Cube shader
		lightingShaderProgram.use();
		// Set uniforms in shader program
		// Model matrix for world-centered cube
		model_matrix = glm::mat4(1.0f);
		lightingShaderProgram.setMatrix4("model", model_matrix);
		// Use same view and proj matrices as for lamp (above)
		lightingShaderProgram.setMatrix4("view", view_matrix);
		lightingShaderProgram.setMatrix4("proj", projection_matrix);
		lightingShaderProgram.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		// Set material struct properties
		lightingShaderProgram.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		lightingShaderProgram.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingShaderProgram.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShaderProgram.setFloat("material.shininess", 128.0f);
		// Set light (intensity) struct properties
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f);
		lightingShaderProgram.setVec3("light.ambient", ambientColor);
		lightingShaderProgram.setVec3("light.diffuse", diffuseColor);
		lightingShaderProgram.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		// Light position
		lightingShaderProgram.setVec3("lightPos", lightPos);

		glBindVertexArray(VAO_cube);
		//glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);

		// Cube objects
		for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
		{
			// Model: Render copies of cube with differing model matrices
			glm::mat4 model_matrix(1.0f);
			model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
			model_matrix = glm::translate(model_matrix, cubePositions[i] * (float)(sin(glfwGetTime()) / 2.0f + 0.5f));
			float twistSpeed = i / 2.0f + 7.0f;
			model_matrix = glm::rotate(model_matrix, twistSpeed*(float)(sin(glfwGetTime()) / 2.0f + 0.5f), glm::vec3(0.1f, 0.1f, 0.15f));
			// View: Translate scene in reverse direction from camera
			glm::mat4 view_matrix = camera.GetViewMatrix();
			// Proj: Zoom/Field of View (FOV), set aspect ratio, front and back clipping of view frustum 
			glm::mat4 projection_matrix(1.0f);
			projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
			// Set uniforms in shader program
			lightingShaderProgram.setMatrix4("model", model_matrix);
			lightingShaderProgram.setMatrix4("view" , view_matrix);
			lightingShaderProgram.setMatrix4("proj" , projection_matrix);

			glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);
		}

		// Check events and swap frame buffers (avoids flickering)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int numAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
	std::cout << "Maximum number of vertex attributes supported: " << numAttributes << std::endl;

	// Release GLFW resources before exiting
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteVertexArrays(1, &VAO_light);
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

	// Camera position movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	// Fixes first mouse cursor capture by OpenGL window
	// TODO: figure out why the camera still jumps
	if (firstMouseCapture)
	{
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstMouseCapture = false;
	}
	float xOffset = xpos - lastCursorX;
	float yOffset = lastCursorY - ypos; // reverse the y-coordinates
	float cursorSensitivity = 0.08f;
	xOffset *= cursorSensitivity;
	yOffset *= cursorSensitivity;
	yaw += xOffset;
	pitch += yOffset;
	lastCursorX = xpos;
	lastCursorY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}