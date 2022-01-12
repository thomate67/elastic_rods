#include "RenderCube.h"

RenderCube::RenderCube()
{
	m_size = 1.0f;
	m_center = glm::vec4(glm::vec3(0.0f), 1.0f);
	create(m_size);
}

RenderCube::RenderCube(float size)
{
	m_size = size;
	m_center = glm::vec4(glm::vec3(0.0f), 1.0f);
	create(m_size);
}

RenderCube::RenderCube(float size, glm::vec3 center)
{
	m_size = size;
	m_center = glm::vec4(center, 1.0f);
	create(m_size);
}

RenderCube::~RenderCube()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteBuffers(1, &m_normalbuffer);
	glDeleteBuffers(1, &m_uvbuffer);
	glDeleteBuffers(1, &m_indexlist);

	glDeleteVertexArrays(1, &m_vao);
}

void RenderCube::createBuffers()
{
	m_points = m_vertices.size();
	m_indices = m_index.size();

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_indexlist);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexlist);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices * sizeof(unsigned int), &m_index[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexlist);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void RenderCube::render()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderCube::render(int n)
{
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0, n);
	glBindVertexArray(0);
}

void RenderCube::setSize(float size) 
{

	m_vertices.clear();

	GLfloat vertices[] = 
	{
		-1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
		-1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
		1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f,
		1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f 
	};

	for (int i = 0; i < m_points; i++) 
	{
		m_vertices.push_back(glm::vec4(vertices[i * 3] * size,
			vertices[i * 3 + 1] * size,
			vertices[i * 3 + 2] * size, 1.0f));
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec4), &m_vertices[0],
		GL_STATIC_DRAW);
}

std::vector<glm::vec4> RenderCube::getVertices()
{
	return m_vertices;
}

std::vector<glm::vec3> RenderCube::getNormals()
{
	return m_normals;
}

std::vector<glm::vec2>& RenderCube::getUVs()
{
	return m_uvs;
}

std::vector<unsigned int>& RenderCube::getIndex()
{
	return m_index;
}

GLuint RenderCube::getVAO()
{
	return m_vao;
}

int RenderCube::getNumIndices() const
{
	return m_indices;
}

float RenderCube::getSize()
{
	return m_size;
}

glm::vec3 RenderCube::getCenter()
{
	return glm::vec3(m_center);
}

// A -1.0f,  1.0f,  1.0f
// B -1.0f, -1.0f,  1.0f
// C  1.0f, -1.0f,  1.0f
// D  1.0f,  1.0f,  1.0f
// E  1.0f,  1.0f, -1.0f
// F  1.0f, -1.0f, -1.0f
// G -1.0f, -1.0f, -1.0f
// H -1.0f,  1.0f, -1.0f

// A,  B,  C,  D, Front
// H,  G,  B,  A, Left
// D,  C,  F,  E, Right
// H,  A,  D,  E, Top
// B,  G,  F,  C, Bottom
// E,  F,  G,  H, Back

void RenderCube::create(float size) 
{
	GLfloat vertices[] = 
	{
		-1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
		-1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
		1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f,
		1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f 
	};

	GLfloat normals[] = 
	{ 
		0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 0.0f,  0.0f,  1.0f, -1.0f, 0.0f,  0.0f,  -1.0f,
		0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f,  0.0f,
		1.0f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		0.0f, 1.0f,  0.0f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,  -1.0f,
		0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f,  -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,  -1.0f 
	};

	GLfloat texCoords[] = 
	{
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f 
	};

	m_points = 24;
	m_indices = 36;

	float x, y, z;

	for (int i = 0; i < m_points; i++) 
	{
		//m_vertices.push_back(m_center + glm::vec4(vertices[i * 3] * size,
		//	vertices[i * 3 + 1] * size,
		//	vertices[i * 3 + 2] * size, 1.0f));
		x = m_center.x + vertices[i * 3] * size;
		y = m_center.y + vertices[i * 3 + 1] * size;
		z = m_center.z + vertices[i * 3 + 2] * size;
		m_vertices.push_back(glm::vec4(x, y, z, 1.0f));
		//m_vertices.push_back(glm::vec4(vertices[i * 3] * size,
		//	vertices[i * 3 + 1] * size,
		//	vertices[i * 3 + 2] * size, 1.0f));
		m_normals.push_back(
			glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]) - glm::vec3(m_center));
		m_uvs.push_back(glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]));
	}

	for (int i = 0; i < 6; i++) 
	{
		m_index.push_back(i * 4 + 0);
		m_index.push_back(i * 4 + 1);
		m_index.push_back(i * 4 + 2);
		m_index.push_back(i * 4 + 2);
		m_index.push_back(i * 4 + 3);
		m_index.push_back(i * 4 + 0);
	}

	createBuffers();
}

void RenderCube::updateVBO(glm::vec3 vel)
{
	for (int i = 0; i < m_vertices.size(); i++)
	{
		m_vertices[i] += glm::vec4(vel, 0.0f);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);
}