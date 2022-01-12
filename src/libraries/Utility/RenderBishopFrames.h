#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class RenderBishopFrames
{
public:
	RenderBishopFrames(std::vector<glm::vec4> vertices, std::vector<glm::vec3> edges, std::vector<glm::vec3> tangents, std::vector<glm::vec3> normals, std::vector<glm::vec3> binormals);
	virtual ~RenderBishopFrames();

	void createBuffers();
	void renderTangents();
	void renderNormals();
	void renderBinormals();

protected:
	GLuint m_vao1;
	GLuint m_vao2;
	GLuint m_vao3;

	GLuint m_vbo1;
	GLuint m_vbo2;
	GLuint m_vbo3;

	std::vector<glm::vec3> m_center;
	std::vector<glm::vec4> m_vertTangents;
	std::vector<glm::vec4> m_vertNormals;
	std::vector<glm::vec4> m_vertBinormals;

private:
	std::vector<glm::vec3> m_edges;
	std::vector<glm::vec3> m_tangents;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_binormals;

	void setCenterPoints(std::vector<glm::vec4> vertices);
	void setVertices();
};