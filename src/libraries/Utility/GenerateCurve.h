#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class GenerateCurve
{
public:
	GenerateCurve(glm::vec3 center, float a, float length, int vertCount, int type);
	virtual ~GenerateCurve();

	void createBuffers();
	void render(int type);

	void createCurve(int type);

	inline void setCenterPoint(glm::vec3 center)
	{
		m_center = center;
		m_vertices.push_back(glm::vec4(m_center, 1.0f));
	}
	inline glm::vec3 getCenterPoint()
	{
		return m_center;
	}

	inline void setA(float a)
	{
		m_a = a;
	}
	inline float getA()
	{
		return m_a;
	}

	inline void setLength(float length)
	{
		m_length = length;
	}
	inline float getLength()
	{
		return m_length;
	}

	inline void setVertCount(int vertCount)
	{
		m_vertCount = vertCount;
	}
	inline int getVertCount()
	{
		return m_vertCount;
	}

	inline std::vector<glm::vec4> getVertices()
	{
		return m_vertices;
	}

protected:
	GLuint m_vao;
	GLuint m_vertexbuffer;

	std::vector<glm::vec4> m_vertices;

private:
	glm::vec3 m_center;
	float m_length;
	int m_vertCount;
	float m_a;
};