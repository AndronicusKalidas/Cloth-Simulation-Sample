#pragma once

#include "mesh.h"

class CClothMesh :
	public CMesh
{
public:
	CClothMesh(int& width, int& height, float spacing);
	~CClothMesh();
	
	void ResetMesh();
	void UpdateMesh();
	void UpdatePoint(int col, int row, glm::vec3 newLoc);

	void RemoveEdge(glm::ivec2 point1, glm::ivec2 point2);

	GLfloat* GetBufferData();
	int GetBufferLength();

	GLuint* GetIndiceData();
	int GetIndiceLength();

private:
	GLuint VBO;
	GLuint EBO;

	GLuint* Indices;
	int IndiceLength;

	// Point, Normal, TextureData
	GLfloat* BufferData;
	int BufferLength;

	float Spacing;
	int& Width;
	int& Height;
	

};

