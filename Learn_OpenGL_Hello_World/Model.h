#pragma once

// CREDIT TO https://learnopengl.com/book/learnopengl_book.pdf for the base Mesh class source code

#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <Shader.h>
#include <Mesh.h>
using namespace std;

class Model
{
public:

	// Constructor
	Model(char* path)
	{
		loadModel(path);
	}

	// Draw all the model's meshes
	void Draw(Shader shaderProgram)
	{
		for (unsigned int i = 0; i < this->meshes.size(); i++)
			meshes[i].Draw(shaderProgram);
	}

private:

	// Model Data 
	vector<Mesh> meshes; 
	string directory;
	vector<Texture> textures_loaded;

	// Import model into memory using assimp
	void loadModel(string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	// Recursively process assimp mesh nodes, then their children
	void processNode(aiNode* node, const aiScene* scene)
	{
		// process all of this node’s meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// 3 sections to processing a mesh:
		// - retrieving all the vertex data 
		// - retrieving the mesh’s indices
		// - retrieving the relevant material data
		vector<Vertex> meshVertices;
		vector<unsigned int> meshIndices;
		vector<Texture> meshTextures;
		// For each vertex in the mesh ...
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex meshVertex;
			// process the vertex's positions
			glm::vec3 posVector;
			posVector.x = mesh->mVertices[i].x;
			posVector.y = mesh->mVertices[i].y;
			posVector.z = mesh->mVertices[i].z;
			meshVertex.Position = posVector;
			// process the vertex's  normals 
			glm::vec3 normVector;
			normVector.x = mesh->mNormals[i].x;
			normVector.y = mesh->mNormals[i].y;
			normVector.z = mesh->mNormals[i].z;
			meshVertex.Normal = normVector;
			// process the vertex's texture coords
			if (mesh->mTextureCoords[0]) 
			{
				glm::vec2 texCoordsVec;
				texCoordsVec.x = mesh->mTextureCoords[0][i].x;
				texCoordsVec.y = mesh->mTextureCoords[0][i].y;
				meshVertex.TexCoords = texCoordsVec;
			} else {
				meshVertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			// push the vertex
			meshVertices.push_back(meshVertex);
		}
		// Process all the mesh's indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				meshIndices.push_back(face.mIndices[j]);
		}

		// Process the mesh's material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			meshTextures.insert(meshTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			meshTextures.insert(meshTextures.end(), specularMaps.begin(), specularMaps.end());
		}
		return Mesh(meshVertices, meshIndices, meshTextures);
	}

	// Loads textures if they're not loaded yet. Data is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // texture is already loaded into memory
					break;
				}
			}
			if (!skip)
			{ // Only load if texture hasn’t been loaded already
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				textures_loaded.push_back(texture); // add to loaded textures
			}
		}
		return textures;
	}

	unsigned int TextureFromFile(const char* path, const string& directory)
	{
		string filepath = string(path);
		filepath = directory + '/' + filepath;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int texWidth, texHeight, numComponents;
		unsigned char* data = stbi_load(filepath.c_str(), &texWidth, &texHeight, &numComponents, 0);
		if (data)
		{
			GLenum textureFormat;
			if (numComponents == 1)
				textureFormat = GL_RED;
			else if (numComponents == 3)
				textureFormat = GL_RGB;
			else if (numComponents == 4)
				textureFormat = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, texWidth, texHeight, 0, textureFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
		return textureID;
	}
};
#endif
