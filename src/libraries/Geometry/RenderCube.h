#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>
#include <iostream>

class RenderCube
{
public:
	RenderCube();
	RenderCube(float size);
	RenderCube(float size, glm::vec3 center);
	virtual ~RenderCube();

	void createBuffers();

	void updateVBO(glm::vec3 vel);
	void render();
	void render(int n);

	void setSize(float size);

	std::vector<glm::vec4> getVertices();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec2>& getUVs();
	std::vector<unsigned int>& getIndex();
	GLuint getVAO();
	int getNumIndices() const;
	float getSize();
	glm::vec3 getCenter();

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
	void create(float size);
	float m_size;
	glm::vec4 m_center;
};