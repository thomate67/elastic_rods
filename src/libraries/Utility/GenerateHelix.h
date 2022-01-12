#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class GenerateHelix
{
public:
	GenerateHelix(glm::vec3 center, float length, int vertCount, int type);
	virtual ~GenerateHelix();

	void createBuffers();
	void render(int type);

	void createLine(int type);

	inline void setCenterPoint(glm::vec3 center)
	{
		m_center = center;
	}
	inline glm::vec3 getCenterPoint()
	{
		return m_center;
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
};