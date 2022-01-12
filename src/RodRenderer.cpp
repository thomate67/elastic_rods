#include "RodRenderer.h"

RodRenderer::RodRenderer()
{

}


RodRenderer::~RodRenderer()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteBuffers(1, &m_normalbuffer);
	glDeleteBuffers(1, &m_uvbuffer);
	glDeleteBuffers(1, &m_indexlist);

	glDeleteVertexArrays(1, &m_vao);
}

void RodRenderer::createBuffer()
{
	if (!m_highRes)
	{
		glGenBuffers(1, &m_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}
	else if (m_highRes)
	{
		RenderCone tube = RenderCone();
	}
}

void RodRenderer::deleteBuffer()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteBuffers(1, &m_normalbuffer);
	glDeleteBuffers(1, &m_uvbuffer);
	glDeleteBuffers(1, &m_indexlist);

	glDeleteVertexArrays(1, &m_vao);
}

void RodRenderer::render()
{
	if (!m_highRes)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);

		//glLineWidth(5.5f);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, m_vertices.size());
		glBindVertexArray(0);
	}
	else if (m_highRes)
	{
		
	}
}

void RodRenderer::renderVertices()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);

	//glPointSize(15.5f);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_POINTS, 0, m_vertices.size());
	glBindVertexArray(0);
}

void RodRenderer::setVertices(std::vector<glm::vec4> vertices)
{
	m_vertices = vertices;
}

void RodRenderer::setRenderMode(bool mode)
{
	m_highRes = mode;
}
