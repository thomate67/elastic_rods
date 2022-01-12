#include "RenderBishopFrames.h"

RenderBishopFrames::RenderBishopFrames(std::vector<glm::vec4> vertices, std::vector<glm::vec3> edges, std::vector<glm::vec3> tangents, std::vector<glm::vec3> normals, std::vector<glm::vec3> binormals)
{
	m_edges = edges;
	m_tangents = tangents;
	m_normals = normals;
	m_binormals = binormals;

	setCenterPoints(vertices);
	setVertices();
	createBuffers();
}

RenderBishopFrames::~RenderBishopFrames()
{
	glDeleteBuffers(1, &m_vbo1);
	glDeleteVertexArrays(1, &m_vao1);
	glDeleteBuffers(1, &m_vbo2);
	glDeleteVertexArrays(1, &m_vao2);
	glDeleteBuffers(1, &m_vbo3);
	glDeleteVertexArrays(1, &m_vao3);
}

void RenderBishopFrames::createBuffers()
{
	glGenBuffers(1, &m_vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo1);
	glBufferData(GL_ARRAY_BUFFER, m_vertTangents.size() * sizeof(glm::vec4), &m_vertTangents[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao1);
	glBindVertexArray(m_vao1);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo1);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);

	glGenBuffers(1, &m_vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	glBufferData(GL_ARRAY_BUFFER, m_vertNormals.size() * sizeof(glm::vec4), &m_vertNormals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);

	glGenBuffers(1, &m_vbo3);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo3);
	glBufferData(GL_ARRAY_BUFFER, m_vertBinormals.size() * sizeof(glm::vec4), &m_vertBinormals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao3);
	glBindVertexArray(m_vao3);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
}

void RenderBishopFrames::renderTangents()
{
	glLineWidth(2.5f);
	glBindVertexArray(m_vao1);
	glDrawArrays(GL_LINES, 0, m_vertTangents.size());
	glBindVertexArray(0);
}

void RenderBishopFrames::renderNormals()
{
	glLineWidth(2.5f);
	glBindVertexArray(m_vao2);
	glDrawArrays(GL_LINES, 0, m_vertNormals.size());
	glBindVertexArray(0);
}

void RenderBishopFrames::renderBinormals()
{
	glLineWidth(2.5f);
	glBindVertexArray(m_vao3);
	glDrawArrays(GL_LINES, 0, m_vertBinormals.size());
	glBindVertexArray(0);
}

void RenderBishopFrames::setCenterPoints(std::vector<glm::vec4> vertices)
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_center.push_back(glm::vec3(vertices[i].x + m_edges[i].x/2, vertices[i].y + m_edges[i].y/2, vertices[i].z + m_edges[i].z/2));
	}
}

void RenderBishopFrames::setVertices()
{
	for (int i = 0; i < m_tangents.size(); i++)
	{
		m_vertTangents.push_back(glm::vec4(m_center[i], 1.0f));
		m_vertTangents.push_back(glm::vec4(m_center[i] + 0.1f * m_tangents[i], 1.0f));
	}

	for (int i = 0; i < m_normals.size(); i++)
	{
		m_vertNormals.push_back(glm::vec4(m_center[i], 1.0f));
		m_vertNormals.push_back(glm::vec4(m_center[i] + 0.1f * m_normals[i], 1.0f));
	}

	for (int i = 0; i < m_binormals.size(); i++)
	{
		m_vertBinormals.push_back(glm::vec4(m_center[i], 1.0f));
		m_vertBinormals.push_back(glm::vec4(m_center[i] + 0.1f * m_binormals[i], 1.0f));
	}
}
