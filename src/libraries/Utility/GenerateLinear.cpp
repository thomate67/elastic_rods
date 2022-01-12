#include "GenerateLinear.h"

GenerateLinear::GenerateLinear(glm::vec3 start, float length, int vertCount, int type)
{
	m_start = start;
	m_length = length;
	m_vertCount = vertCount;

	createLine(type);
	createBuffers();
}

GenerateLinear::~GenerateLinear()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteVertexArrays(1, &m_vao);
}

void GenerateLinear::createBuffers()
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

void GenerateLinear::render(int type)
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

void GenerateLinear::createLine(int type)
{
	m_vertices.push_back(glm::vec4(m_start, 1.0f));
	float distr = m_length / m_vertCount;

	switch (type)
	{
	case 0:																	//generation of the line along the x axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x + i * distr;
			float newY = m_start.y;
			float newZ = m_start.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 1:																	//generation of the line along the y axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x;
			float newY = m_start.y - i * distr;
			float newZ = m_start.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 2:																	//generation of the line along the z axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x;
			float newY = m_start.y;
			float newZ = m_start.z + i * distr;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 3:																	//generation of the line along the xy / yx axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x + i * distr;
			float newY = m_start.y + i * distr;
			float newZ = m_start.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 4:																	//generation of the line along the xz / zx axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x + i * distr;
			float newY = m_start.y;
			float newZ = m_start.z + i * distr;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 5:																	//generation of the line along the yz / zy axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x;
			float newY = m_start.y + i * distr;
			float newZ = m_start.z + i * distr;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 6:																	//generation of the line along the xyz axis
		for (int i = 1; i < m_vertCount; i++)
		{
			float newX = m_start.x + i * distr;
			float newY = m_start.y + i * distr;
			float newZ = m_start.z + i * distr;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	default:
		break;
	}
}