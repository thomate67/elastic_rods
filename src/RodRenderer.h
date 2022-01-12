#pragma once

#include <src/libraries/Utility/Definitions.h>
#include <src/libraries/Math/MathFunctions.h>
#include<src/libraries/Geometry/RenderSphere.h>
#include<src/libraries/Geometry/RenderCone.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <iostream>

class RodRenderer
{
public:
	RodRenderer();
	virtual ~RodRenderer();

	void createBuffer();
	void deleteBuffer();
	void render();
	void renderVertices();

	void setVertices(std::vector<glm::vec4> vertices);
	void setRenderMode(bool mode);


protected:
	GLuint m_vao;
	GLuint m_vertexbuffer;
	GLuint m_normalbuffer;
	GLuint m_uvbuffer;
	GLuint m_indexlist;

	int m_points;
	int m_indices;

	std::vector<glm::vec4> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;
	std::vector<unsigned int> m_index;

private:
	bool m_highRes = false;					//m_highRes determines the render mode, if false, the rods will be visualized with GL_LINES
											// if true, than the rods are detailed with cylinders and a more komplex representation
};