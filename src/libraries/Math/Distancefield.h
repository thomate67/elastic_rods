#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <iostream>
#include <src/libraries/Utility/definitions.h>

class Distancefield
{
public:
	static const float sdSphere(glm::vec3 p, float r);
	static const float sdBox(glm::vec3 p, glm::vec3 b);
	static const float sdSphere(glm::vec3 p, float r, glm::mat4 modelMatrix);
	static const float sdBox(glm::vec3 p, glm::vec3 b, glm::mat4 modelMatrix);
	static const float sdPlane(glm::vec3 p, glm::vec3 n, float h);
	static const float sdPlane(glm::vec3 p, glm::mat4 modelMatrix);

	static const glm::vec3 sdSphereGrad(glm::vec3 p, float r);
	static const glm::vec3 sdBoxGrad(glm::vec3 p, glm::vec3 b);
	static const glm::vec3 sdPlaneGrad(glm::vec3 p, glm::vec3 n, float h);
	static const glm::vec3 sdSphereGrad(glm::vec3 p, float r, glm::mat4 modelMatrix);
	static const glm::vec3 sdBoxGrad(glm::vec3 p, glm::vec3 b, glm::mat4 modelMatrix);
	static const glm::vec3 sdPlaneGrad(glm::vec3 p, glm::mat4 modelMatrix);
private:
	float m_radius;
	glm::vec3 m_b;
	glm::vec3 m_n;
	float m_h;
};