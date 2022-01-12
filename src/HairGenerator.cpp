#include "HairGenerator.h"

HairGenerator::HairGenerator(glm::vec3 root, float length, int vertCount, int type)
{
	m_root = root;
	//m_length = length mit length variance anpassen
	m_length = length;
	m_numVertices = vertCount;
	m_type = type;

	if (type == 0)
	{
		generateStraightHair();
	}
	else if (type == 1)
	{
		generateCurlyHair();
	}
}

HairGenerator::~HairGenerator()
{

}

void HairGenerator::generateStraightHair()
{
	m_vertices.push_back(glm::vec4(m_root, 1.0f));
	float distr = m_length / m_numVertices;

	for (int i = 1; i < m_numVertices; i++)
	{
		float newX = m_root.x + i * distr;
		float newY = m_root.y;
		float newZ = m_root.z;
		//glm::vec3 newPos = glm::vec3(m_root) + ((i * distr) * m_normal);

		m_vertices.push_back(glm::vec4(newX, newY, newZ, 1.0f));
		//m_vertices.push_back(glm::vec4(newPos, 1.0f));
	}
	
	/*m_vertices.resize(m_numVertices);
	glm::vec3 endPos = glm::vec3(m_root) + (m_length * glm::vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < m_numVertices; i++)
	{
		float t = static_cast<float>(i) / m_numVertices - 1;
		m_vertices[i] = glm::vec4(MathFunctions::lerp(glm::vec3(m_root), endPos, t), 1.0f);
	}*/
}

void HairGenerator::generateCurlyHair()
{
	m_vertices.push_back(glm::vec4(m_root, 1.0f));
	float curve_radius = 0.6f;

	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 dirx = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 diry = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 dirz = glm::vec3(0.0f, 0.0f, 1.0f);

	auto angle = m_length / ((m_numVertices - 1) * curve_radius);
	auto step = 0.1f;

	for (int i = 1; i < m_numVertices; i++)
	{
		m_vertices.push_back(glm::vec4(glm::vec3((std::cos(i * angle) * dirx + std::sin(i * angle) * dirz - dirx) * curve_radius + center), 1.0f));
		m_vertices[i] += glm::vec4(glm::vec3(diry * static_cast<float>(i) * step), 0.0f);
	}

	glm::vec3 endPos = m_root + m_length * dirx;
	for (int i = 0; i < m_numVertices; i++)
	{
		auto t = static_cast<float>(i) / (m_numVertices - 1);
		m_vertices[i] += glm::vec4(glm::vec3(MathFunctions::lerp(m_root, endPos, t)), 1.0f);
	}
}