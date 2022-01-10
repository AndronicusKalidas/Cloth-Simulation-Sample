#include <array>
#include <glm.hpp>
#include <glm/gtx/normal.hpp>

#include "clothMesh.h"

CClothMesh::CClothMesh(int& width, int& height, float spacing) :
	Width(width),
	Height(height)
{
	Spacing = spacing;

	BufferLength = width * height * 8;
	BufferData = new GLfloat[BufferLength];

	IndiceLength = (width - 1) * (height - 1) * 6;
	Indices = new GLuint[IndiceLength];

	IndiceCount = IndiceLength;

	ResetMesh();


	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndiceLength, Indices, GL_DYNAMIC_DRAW);

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * BufferLength, BufferData, GL_DYNAMIC_DRAW);

	// Position
	glVertexAttribPointer(
		0,								// Maps to the layout location
		3,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)0						// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(0);

	// Normal
	glVertexAttribPointer(
		1,								// Maps to the layout location
		3,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)(3 * sizeof(GLfloat))	// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(1);

	// Texture Coordinates
	glVertexAttribPointer(
		2,								// Maps to the layout location
		2,								// Number of components
		GL_FLOAT,						// Type
		GL_FALSE,						// Should the shader normalise the input
		8 * sizeof(GLfloat),			// Step Size
		(GLvoid*)(6 * sizeof(GLfloat))	// Offset from beginning of Vertex
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

CClothMesh::~CClothMesh()
{
	delete[] BufferData;
	delete[] Indices;
}

void CClothMesh::ResetMesh()
{
	delete[] BufferData;
	delete[] Indices;

	BufferLength = Width * Height * 8;
	BufferData = new GLfloat[BufferLength];

	IndiceLength = (Width - 1) * (Height - 1) * 6;
	Indices = new GLuint[IndiceLength];

	IndiceCount = IndiceLength;
	
	const float zOffset = -Width * Spacing * 0.5f;
	const float yOffset = -Height * Spacing * 0.5f;

	float zLoc = 0.0f;
	float yLoc = 0.0f;

	float xUV = 0.0f;
	float yUV = 0.0f;

	for (int z = 0; z < Width; ++z)
	{
		xUV = (z / (float)Width);
		zLoc = xUV * Width * Spacing + zOffset;

		for (int y = 0; y < Height; ++y)
		{
			int index = y * Width * 8 + z * 8;

			yUV = (y / (float)Height);
			yLoc = yUV * Height * Spacing + yOffset;

			BufferData[index] = 0.0f;					// position x
			BufferData[index + 1] = zLoc;				// position y
			BufferData[index + 2] = yLoc;				// position z

			BufferData[index + 3] = 1.0f;				// normal x
			BufferData[index + 4] = 0.0f;				// normal y
			BufferData[index + 5] = 0.0f;				// normal z

			BufferData[index + 6] = xUV;				// UV x
			BufferData[index + 7] = yUV;				// UV y
		}
	}

	int index = 0;

	for (int z = 0; z < Height - 1; ++z)
	{
		for (int x = 0; x < Width - 1; ++x)
		{
			Indices[index + 0] = z * Width + x;				// Current
			Indices[index + 1] = z * Width + x + 1;			// Right
			Indices[index + 2] = (z + 1) * Width + x;		// Below

			Indices[index + 3] = (z + 1) * Width + x;		// Below
			Indices[index + 4] = z * Width + x + 1;			// Right
			Indices[index + 5] = (z + 1) * Width + x + 1;	// Bot Right

			index += 6;
		}
	}
}

void CClothMesh::UpdateMesh()
{
	glBindVertexArray(VAO);

	// Update VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * BufferLength, BufferData, GL_DYNAMIC_DRAW);

	// Update EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndiceLength, Indices, GL_DYNAMIC_DRAW);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void CClothMesh::UpdatePoint(int col, int row, glm::vec3 newLoc)
{
	// Set new point
	int index0 = row * Width * 8 + col * 8; // center

	BufferData[index0] = newLoc.x;
	BufferData[index0 + 1] = newLoc.y;
	BufferData[index0 + 2] = newLoc.z;

	// Get surrounding indexs
	int index1 = index0 - 8;				// left
	int index2 = index0 + 8;				// right
	int index3 = index0 + 8 * Width;		// down
	int index4 = index0 - 8 * Width;		// up

	// Keep in bounds
	index1 = index1 < 0 ? index0 : index1;
	index2 = index2 >= BufferLength ? index0 : index2;
	index3 = index3 >= BufferLength ? index0 : index3;
	index4 = index4 < 0 ? index0 : index4;

	// Get points
	glm::vec3 p0 = newLoc;
	glm::vec3 p1 = glm::vec3(BufferData[index1], BufferData[index1 + 1], BufferData[index1 + 2]);
	glm::vec3 p2 = glm::vec3(BufferData[index2], BufferData[index2 + 1], BufferData[index2 + 2]);
	glm::vec3 p3 = glm::vec3(BufferData[index3], BufferData[index3 + 1], BufferData[index3 + 2]);
	glm::vec3 p4 = glm::vec3(BufferData[index4], BufferData[index4 + 1], BufferData[index4 + 2]);

	// Find surrounding triangles normals
	glm::vec3 normal1 = p1 == p0 || p4 == p0 ? glm::vec3() : glm::triangleNormal(p1, p4, p0); // top left
	glm::vec3 normal2 = p1 == p0 || p3 == p0 ? glm::vec3() : glm::triangleNormal(p1, p0, p3); // bot left
	glm::vec3 normal3 = p2 == p0 || p4 == p0 ? glm::vec3() : glm::triangleNormal(p0, p4, p2); // top right
	glm::vec3 normal4 = p2 == p0 || p3 == p0 ? glm::vec3() : glm::triangleNormal(p0, p2, p3); // bot right

	// Calculate the new average normal
	glm::vec3 normal = glm::normalize(normal1 + normal2 + normal3 + normal4);

	// Set normal to the point
	BufferData[index0 + 3] = normal.x;
	BufferData[index0 + 4] = normal.y;
	BufferData[index0 + 5] = normal.z;
}

void CClothMesh::RemoveEdge(glm::ivec2 point1, glm::ivec2 point2)
{
	// Point 1
	int x1 = point1.x;
	int y1 = point1.y;

	// Point 2
	int x2 = point2.x;
	int y2 = point2.y;

	// Index of points
	int i1 = y1 * Width + x1;
	int i2 = y2 * Width + x2;

	// Boundries
	int w = Width - 1;
	int h = Height - 1;

	// Calculate the number of triangles needed to be cut
	int maximum = ((x1 == 0 || x1 == w || y1 == 0 || y1 == h) && (x2 == 0 || x2 == w || y2 == 0 || y2 == h)) ? 1 : 2;
	int current = 0;

	// Loop through each triangle
	for (int i = 0; i < IndiceLength; i += 3)
	{
		// Get indices
		int indice1 = Indices[i];
		int indice2 = Indices[i + 1];
		int indice3 = Indices[i + 2];

		// Check if they contain both points
		bool containsA = (indice1 == i1 || indice2 == i1 || indice3 == i1);
		bool containsB = (indice1 == i2 || indice2 == i2 || indice3 == i2);

		if (containsA && containsB)
		{
			// Delete the triangle
			Indices[i] = 0;
			Indices[i + 1] = 0;
			Indices[i + 2] = 0;

			// Check if all triangles have been cut
			++current;
			if (current == maximum)
			{
				return;
			}
		}
	}
}

GLfloat* CClothMesh::GetBufferData()
{
	return BufferData;
}

int CClothMesh::GetBufferLength()
{
	return BufferLength;
}

GLuint* CClothMesh::GetIndiceData()
{
	return Indices;
}

int CClothMesh::GetIndiceLength()
{
	return IndiceLength;
}