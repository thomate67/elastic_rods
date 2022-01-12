#include "GenerateCurve.h"

GenerateCurve::GenerateCurve(glm::vec3 center,float a, float length, int vertCount, int type)
{
	m_center = center;
	m_a = a;
	m_length = length;
	m_vertCount = vertCount;

	createCurve(type);
	createBuffers();
}

GenerateCurve::~GenerateCurve()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteVertexArrays(1, &m_vao);
}

void GenerateCurve::createBuffers()
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

void GenerateCurve::render(int type)
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

void GenerateCurve::createCurve(int type)
{
	float distr = m_length / m_vertCount;

	switch (type)
	{
	case 0:																	//generation of the parabel along the x axis
		for (int i = m_vertCount / 2; i > 0; i--)
		{
			float newX = m_center.x + (-(i * distr));
			float newY = m_center.y + (m_a * ((-i * distr) * (-i * distr)));
			float newZ = m_center.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}


		for (int i = 0; i <= (m_vertCount / 2); i++)
		{
			float newX = m_center.x + i * distr;
			float newY = m_center.y + (m_a * ((i * distr) * (i * distr)));
			float newZ = m_center.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 1:																	//generation of the parabel along the y axis
		for (int i = m_vertCount / 2; i > 0; i--)
		{
			float newX = m_center.x + (m_a * ((-i * distr) * (-i * distr))); 
			float newY = m_center.y + (-(i * distr));
			float newZ = m_center.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}


		for (int i = 0; i <= (m_vertCount / 2); i++)
		{
			float newX = m_center.x + (m_a * ((i * distr) * (i * distr))); 
			float newY = m_center.y + i * distr;
			float newZ = m_center.z;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	case 2:																	//generation of the parabel along the x axis
		for (int i = m_vertCount / 2; i > 0; i--)
		{
			float newX = m_center.x;
			float newY = m_center.y + (m_a * ((-i * distr) * (-i * distr)));
			float newZ = m_center.z + (-(i * distr));

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}


		for (int i = 0; i <= (m_vertCount / 2); i++)
		{
			float newX = m_center.x;
			float newY = m_center.y + (m_a * ((i * distr) * (i * distr)));
			float newZ = m_center.z + i * distr;

			m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		}
		break;
	default:
		break;
	}
}