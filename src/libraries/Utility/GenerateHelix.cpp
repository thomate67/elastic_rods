#include "GenerateHelix.h"

GenerateHelix::GenerateHelix(glm::vec3 center, float length, int vertCount, int type)
{
	m_center = center;
	m_length = length;
	m_vertCount = vertCount;

	createLine(type);
	createBuffers();
}

GenerateHelix::~GenerateHelix()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteVertexArrays(1, &m_vao);
}

void GenerateHelix::createBuffers()
{
	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertCount * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
}

void GenerateHelix::render(int type)
{
	glBindVertexArray(m_vao);
	switch (type)
	{
	case 0:
		glDrawArrays(GL_POINTS, 0, m_vertCount);
		break;
	case 1:
		glDrawArrays(GL_LINE_STRIP, 0, m_vertCount);
	default:
		break;
	}
	glBindVertexArray(0);
}

void GenerateHelix::createLine(int type)
{
	float distr = m_length / m_vertCount;

	switch (type)
	{
	case 0:																	//generation of a helix along the x axis
		for (int i = 0; i < m_vertCount; i++)
		{
			float newX = m_center.x + i * distr;
			float newY = m_center.y + (std::cos(i) / (m_length / 2));
			float newZ = m_center.z + (std::sin(i) / (m_length / 2));

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 1:																	//generation of a helix along the y axis
		for (int i = 0; i < m_vertCount; i++)
		{
			float newX = m_center.x + (std::cos(i) / (m_length / 2));
			float newY = m_center.y - i * distr;
			float newZ = m_center.z + (std::sin(i) / (m_length / 2));

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 2:																	//generation of a circle
		for (int i = 0; i < m_vertCount; i++)
		{
			float newX = m_center.x + (std::cos(i * distr));
			float newY = m_center.y + (std::sin(i * distr));
			float newZ = m_center.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	default:
		break;
	}
}