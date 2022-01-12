#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <src/libraries/Geometry/RenderCube.h>
#include <src/libraries/Geometry/RenderSphere.h>

class RigidBody
{
public:
	RigidBody(RenderCube* cube, glm::vec3 pos);
	RigidBody(RenderCube* cube, glm::vec3 pos, float mass);
	RigidBody(RenderSphere* sphere, glm::vec3 pos);
	RigidBody(RenderSphere* sphere, glm::vec3 pos, float mass);
	virtual ~RigidBody();

	void applyForce(const glm::vec3& force);
	void applyForce(const glm::vec3& force, glm::vec3 position);
	void applyTorque(const glm::vec3& torque);

	void integrate(float dt);

	float getMass();

	glm::quat getRotation();
	void setRotation(glm::quat rotation);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);

	glm::vec3 getLinearVelocity();
	void setLinearVelocity(glm::vec3 linVel);

private:
	//variables
	float m_mass;									// mass of the rigid body	
	float m_massInverse;							// inverse of m_mass 1.0f / m_mass
	glm::mat3x3 m_Ibody{};							// inertia tensor in body space
	glm::mat3x3 m_IbodyInverse{};					// inverse of Ibody
	glm::quat m_rotation{};                         // quaternion for orientation
	glm::mat3x3 m_orientation{};					// R(t) angular orientation in world space

	glm::vec3 m_position{};							// x(t) linear position in world space
	glm::vec3 m_linearVelocity{};					// v(t) linear velocity
	glm::vec3 m_angularVelocity{};					// omega(t) angular velocity
	glm::vec3 m_linearMomentum{};					// P(t) linear momentum
	glm::vec3 m_angularMomentum{};					// L(t) angular momentum
	glm::mat3x3 m_inverseInertiaTensor{};			// I^-1(t) inverse of I(t); I(t) = R(t) * Ibody * R(t)^T

	glm::vec3 m_force{};							// F(t) total external force
	glm::vec3 m_torque{};							// tau(t) total external torque

	RenderCube* m_cube{};
	RenderSphere* m_sphere{};
	//glm::mat3x3 m_inertiaTensor;					// inertia Tensor of the rigid body in world space

	float m_linearDamping;
	float m_angularDamping;
};