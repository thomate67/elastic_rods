#include "Distancefield.h"

const float Distancefield::sdSphere(glm::vec3 p, float r)
{
	//m_radius = r;
	return glm::length(p) - r;
}

const float Distancefield::sdBox(glm::vec3 p, glm::vec3 b)
{
	//m_b = b;
	glm::vec3 q = glm::abs(p) - b;
	return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

const float Distancefield::sdSphere(glm::vec3 p, float r, glm::mat4 modelMatrix)
{
	glm::vec3 invt = glm::vec3((glm::inverse(modelMatrix) * glm::vec4(p, 1)));
	return glm::length(invt) - r;
}

const float Distancefield::sdBox(glm::vec3 p, glm::vec3 b, glm::mat4 modelMatrix)
{
	glm::vec3 invt = glm::vec3((glm::inverse(modelMatrix) * glm::vec4(p, 1)));
	glm::vec3 q = glm::abs(invt) - b;
	return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

const float Distancefield::sdPlane(glm::vec3 p, glm::vec3 n, float h)
{
	//m_n = n;
	//m_h = h;
	// n  must be normalized
	n = glm::normalize(n);

	return glm::dot(p, n) + h;
}

const float Distancefield::sdPlane(glm::vec3 p, glm::mat4 modelMatrix)
{
	glm::vec3 invt = glm::vec3((glm::inverse(modelMatrix) * glm::vec4(p, 1)));
	return invt.y;
}

const glm::vec3 Distancefield::sdSphereGrad(glm::vec3 p, float r) {
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdSphere(glm::vec3(p.x + EPSILON, p.y, p.z), r) - sdSphere(glm::vec3(p.x - EPSILON, p.y, p.z), r),
		sdSphere(glm::vec3(p.x, p.y + EPSILON, p.z), r) - sdSphere(glm::vec3(p.x, p.y - EPSILON, p.z), r),
		sdSphere(glm::vec3(p.x, p.y, p.z + EPSILON), r) - sdSphere(glm::vec3(p.x, p.y, p.z - EPSILON), r)
	));
}

const glm::vec3 Distancefield::sdBoxGrad(glm::vec3 p, glm::vec3 b) {
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdBox(glm::vec3(p.x + EPSILON, p.y, p.z), b) - sdBox(glm::vec3(p.x - EPSILON, p.y, p.z), b),
		sdBox(glm::vec3(p.x, p.y + EPSILON, p.z), b) - sdBox(glm::vec3(p.x, p.y - EPSILON, p.z), b),
		sdBox(glm::vec3(p.x, p.y, p.z + EPSILON), b) - sdBox(glm::vec3(p.x, p.y, p.z - EPSILON), b)
	));
}

const glm::vec3 Distancefield::sdPlaneGrad(glm::vec3 p, glm::vec3 n, float h)
{
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdPlane(glm::vec3(p.x + EPSILON, p.y, p.z), n, h) - sdPlane(glm::vec3(p.x - EPSILON, p.y, p.z), n, h),
		sdPlane(glm::vec3(p.x, p.y + EPSILON, p.z), n, h) - sdPlane(glm::vec3(p.x, p.y - EPSILON, p.z), n, h),
		sdPlane(glm::vec3(p.x, p.y, p.z + EPSILON), n, h) - sdPlane(glm::vec3(p.x, p.y, p.z - EPSILON), n, h)
	));
}

const glm::vec3 Distancefield::sdSphereGrad(glm::vec3 p, float r, glm::mat4 modelMatrix) {
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdSphere(glm::vec3(p.x + EPSILON, p.y, p.z), r, modelMatrix) - sdSphere(glm::vec3(p.x - EPSILON, p.y, p.z), r, modelMatrix),
		sdSphere(glm::vec3(p.x, p.y + EPSILON, p.z), r, modelMatrix) - sdSphere(glm::vec3(p.x, p.y - EPSILON, p.z), r, modelMatrix),
		sdSphere(glm::vec3(p.x, p.y, p.z + EPSILON), r, modelMatrix) - sdSphere(glm::vec3(p.x, p.y, p.z - EPSILON), r, modelMatrix)
	));
}

const glm::vec3 Distancefield::sdBoxGrad(glm::vec3 p, glm::vec3 b, glm::mat4 modelMatrix) {
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdBox(glm::vec3(p.x + EPSILON, p.y, p.z), b, modelMatrix) - sdBox(glm::vec3(p.x - EPSILON, p.y, p.z), b, modelMatrix),
		sdBox(glm::vec3(p.x, p.y + EPSILON, p.z), b, modelMatrix) - sdBox(glm::vec3(p.x, p.y - EPSILON, p.z), b, modelMatrix),
		sdBox(glm::vec3(p.x, p.y, p.z + EPSILON), b, modelMatrix) - sdBox(glm::vec3(p.x, p.y, p.z - EPSILON), b, modelMatrix)
	));
}

const glm::vec3 Distancefield::sdPlaneGrad(glm::vec3 p, glm::mat4 modelMatrix)
{
	const float EPSILON = 1e-3;
	return glm::normalize(glm::vec3(
		sdPlane(glm::vec3(p.x + EPSILON, p.y, p.z), modelMatrix) - sdPlane(glm::vec3(p.x - EPSILON, p.y, p.z), modelMatrix),
		sdPlane(glm::vec3(p.x, p.y + EPSILON, p.z), modelMatrix) - sdPlane(glm::vec3(p.x, p.y - EPSILON, p.z), modelMatrix),
		sdPlane(glm::vec3(p.x, p.y, p.z + EPSILON), modelMatrix) - sdPlane(glm::vec3(p.x, p.y, p.z - EPSILON), modelMatrix)
	));
}