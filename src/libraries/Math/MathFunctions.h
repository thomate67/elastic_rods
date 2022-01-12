#pragma once

#include <Eigen/Dense>
#include <src/libraries/Utility/Definitions.h>
#include <glm/gtc/random.hpp>

class MathFunctions
{
public:
	static const glm::mat3x3 skew_symmetric(glm::vec3& v);
	static void tridiagonalSolver(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<float>& d);

	static float lerp(float x, float y, float t);
	static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t);

	//distribute values on 1/4 sphere
	static glm::vec3 quarterSphereRand(glm::vec3 center, float radius);
};

