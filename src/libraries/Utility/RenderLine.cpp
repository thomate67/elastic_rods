#include "RenderLine.h"

RenderLine::RenderLine()
{
	m_start = glm::vec3(0.0f);
	m_end = glm::vec3(1.0f);

	setVertices();
	createBuffers();
}

RenderLine::RenderLine(glm::vec3 start, glm::vec3 end)
{
	m_start = start;
	m_end = end;

	setVertices();
	createBuffers();
}

RenderLine::RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
{
	m_start = start;
	m_end = end;
	m_color = color;

	setVertices();
	createBuffers();
}

RenderLine::~RenderLine()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteVertexArrays(1, &m_vao);
}

void RenderLine::createBuffers()
{
	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
}

void RenderLine::render()
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void RenderLine::render(int n)
{
	glBindVertexArray(m_vao);
	glDrawArraysInstanced(GL_LINES, 0, 2, n);
	glBindVertexArray(0);
}

void RenderLine::setVertices()
{
	m_vertices.push_back(glm::vec4(m_start, 1.0f));
	m_vertices.push_back(glm::vec4(m_end, 1.0f));
}