#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class GenerateLinear
{
public:
	GenerateLinear(glm::vec3 start, float length, int vertCount, int type);
	virtual ~GenerateLinear();

	void createBuffers();
	void render(int type);

	void createLine(int type);			//hier typ für switch case rein

	inline void setStartPoint(glm::vec3 start)
	{
		m_start = start;
		m_vertices.push_back(glm::vec4(m_start, 1.0f));
	}
	inline glm::vec3 getStartPoint()
	{
		return m_start;
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
	glm::vec3 m_start;
	float m_length;
	int m_vertCount;
};