#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class RenderLine
{
public:
	RenderLine();
	RenderLine(glm::vec3 start, glm::vec3 end);
	RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);
	virtual ~RenderLine();

	void createBuffers();
	void render();
	void render(int n);

	inline void setStartPoint(glm::vec3 start)
	{
		m_start = start;
		setVertices();
	}
	inline glm::vec3 getStartPoint() 
	{ 
		return m_start; 
	}

	inline void setEndPoint(glm::vec3 end)
	{
		m_end = end;
		setVertices();
	}
	inline glm::vec3 getEndPoint() 
	{ 
		return m_end; 
	}

	inline void setColor(glm::vec3 color) 
	{ 
		m_color = glm::vec4(color, 1.0f); 
	}
	inline void setColor(glm::vec4 color) 
	{ 
		m_color = color; 
	}
	inline glm::vec4 getColor() 
	{ 
		return m_color; 
	}

protected:
	GLuint m_vao;
	GLuint m_vertexbuffer;

	std::vector<glm::vec4> m_vertices;

private:
	glm::vec3 m_start;		//start position of line
	glm::vec3 m_end;		//end position of line

	glm::vec4 m_color;

	void setVertices();
};