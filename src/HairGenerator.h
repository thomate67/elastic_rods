#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>
#include <src/libraries/Math/MathFunctions.h>

class HairGenerator
{
public:
	HairGenerator(glm::vec3 root, float length, int vertCount, int type);
	virtual ~HairGenerator();

	inline std::vector<glm::vec4> getVertices()
	{
		return m_vertices;
	}
private:
	void generateStraightHair();
	void generateCurlyHair();

	std::vector<glm::vec4> m_vertices;

	glm::vec3 m_root;
	float m_length;
	int m_numVertices;
	//float m_lengthVariance;
	int m_type;

	float m_density = 0.001f;
	float m_thickness = 0.07f;
};