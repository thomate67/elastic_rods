#include "MathFunctions.h"

const glm::mat3x3 MathFunctions::skew_symmetric(glm::vec3& v)
{
	glm::mat3x3 result = glm::mat3x3(0.0f);

	result[0][0] = 0.0f;
	result[1][0] = -v[2];
	result[2][0] = v[1];
	result[0][1] = v[2];
	result[1][1] = 0.0f;
	result[1][1] = -v[0];
	result[0][2] = -v[1];
	result[1][2] = v[0];
	result[2][2] = 0.0f;

	return result;
}

void MathFunctions::tridiagonalSolver(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c, std::vector<float>& d)
{
	/*
	// n is the number of unknowns

	|b0 c0 0 ||x0| |d0|
	|a1 b1 c1||x1|=|d1|
	|0  a2 b2||x2| |d2|

	1st iteration: b0x0 + c0x1 = d0 -> x0 + (c0/b0)x1 = d0/b0 ->

		x0 + g0x1 = r0               where g0 = c0/b0        , r0 = d0/b0

	2nd iteration:     | a1x0 + b1x1   + c1x2 = d1
		from 1st it.: -| a1x0 + a1g0x1        = a1r0
					-----------------------------
						  (b1 - a1g0)x1 + c1x2 = d1 - a1r0

		x1 + g1x2 = r1               where g1=c1/(b1 - a1g0) , r1 = (d1 - a1r0)/(b1 - a1g0)

	3rd iteration:      | a2x1 + b2x2   = d2
		from 2nd it. : -| a2x1 + a2g1x2 = a2r2
					   -----------------------
					   (b2 - a2g1)x2 = d2 - a2r2
		x2 = r2                      where                     r2 = (d2 - a2r2)/(b2 - a2g1)
	Finally we have a triangular matrix:
	|1  g0 0 ||x0| |r0|
	|0  1  g1||x1|=|r1|
	|0  0  1 ||x2| |r2|

	Condition: ||bi|| > ||ai|| + ||ci||

	in this version the c matrix reused instead of g
	and             the d matrix reused instead of r and x matrices to report results
	Written by Keivan Moradi, 2014
	Modified by wy into Eigen manner, 2019
	*/
	int n = (int)a.size();
	assert(n == b.size());
	assert(n == c.size());
	assert(n == d.size());

	n--; // since we start from x0 (not x1)
	c[0] /= b[0];
	d[0] /= b[0];

	for (int i = 1; i < n; i++) {
		c[i] /= b[i] - a[i] * c[i - 1];
		d[i] = (d[i] - a[i] * d[i - 1]) / (b[i] - a[i] * c[i - 1]);
	}

	d[n] = (d[n] - a[n] * d[n - 1]) / (b[n] - a[n] * c[n - 1]);

	for (int i = n; i-- > 0;) {
		d[i] -= c[i] * d[i + 1];
	}
}

float MathFunctions::lerp(float x, float y, float t)
{
	return x * (1.f - t) + y * t;
}

glm::vec3 MathFunctions::lerp(glm::vec3 x, glm::vec3 y, float t)
{
	return x * (1.f - t) + y * t;
}

glm::vec3 MathFunctions::quarterSphereRand(glm::vec3 center, float radius)
{
	float theta = glm::linearRand(0.0f, 1.963495408493620774039f);
	float phi = std::acos(glm::linearRand(-1.0f, 1.0f));

	float x = std::sin(phi) * std::cos(theta);
	float y = std::sin(phi) * std::sin(theta);
	float z = std::cos(phi);

	return glm::vec3(center.x + x, center.y + y, center.z + z) * radius;
}