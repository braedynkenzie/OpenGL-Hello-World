#pragma once

// CREDIT TO https://learnopengl.com/book/learnopengl_book.pdf for the base Mesh class source code

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
/*#include <string>
#include <fstream>
#include <sstream>
#include <iostream>*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <Shader.h>
using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	string type;
	aiString path; // we store the path of the texture to compare with other textures

};

class Mesh {
public:

	// Mesh Data 
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Constructor: takes a vector of vertices and their corresponding indices and texture data vectors
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// Using the given parameters, set the OpenGL vertex buffers and attribute pointers
		setupMesh();
	}

	void Draw(Shader shaderProgram) 
	{
		unsigned int diffuseNum = 1;
		unsigned int specularNum = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				stringstream ss;
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNum++; // transfer unsigned int to stream
			else if (name == "texture_specular")
				ss << specularNum++; // transfer unsigned int to stream
			number = ss.str();
			shaderProgram.setFloat(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:

	// Render data
	unsigned int VAO, VBO, EBO;

	// Functions
	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices, GL_STATIC_DRAW);
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// Unbind VAO
		glBindVertexArray(0);
	}

};

#endif