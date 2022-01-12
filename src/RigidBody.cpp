#include "RigidBody.h"

RigidBody::RigidBody(RenderCube* cube, glm::vec3 pos)
{
	m_cube = cube;
	m_mass = 5.0f;
	m_massInverse = 1.0f / m_mass;
	//m_position = glm::vec3(0.0f);
	//m_orientation = glm::mat3(1.0f);
	m_linearVelocity = glm::vec3(0.0f);
	m_angularVelocity = glm::vec3(0.0f);
	m_linearMomentum = glm::vec3(0.0f);
	m_angularMomentum = glm::vec3(0.0f);

	m_position = pos;
	m_orientation = glm::mat3(1.0f);
	m_rotation = glm::quat(1,0,0,0);
	//m_quatOrientation = glm::quat(0.f, 0.f, 0.f, 0.f);
	//m_orientation = glm::mat3_cast(m_quatOrientation);

	m_Ibody = glm::mat3x3(1.0f);
	m_Ibody[0][0] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));
	m_Ibody[1][1] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));
	m_Ibody[2][2] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));

	m_IbodyInverse = glm::inverse(m_Ibody);

	m_linearDamping = 0.1f;
	m_angularDamping = 0.75f;
}

RigidBody::RigidBody(RenderCube* cube, glm::vec3 pos, float mass)
{
	m_cube = cube;
	m_mass = mass;
	m_massInverse = 1.0f / m_mass;
	//m_position = glm::vec3(0.0f);
	//m_orientation = glm::mat3(1.0f);
	m_linearVelocity = glm::vec3(0.0f);
	m_angularVelocity = glm::vec3(0.0f);
	m_linearMomentum = glm::vec3(0.0f);
	m_angularMomentum = glm::vec3(0.0f);

	m_position = pos;
	m_orientation = glm::mat3(1.0f);
	m_rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	//m_quatOrientation = glm::quat(0.f, 0.f, 0.f, 0.f);
	//m_orientation = glm::mat3_cast(m_quatOrientation);

	m_Ibody = glm::mat3x3(1.0f);
	m_Ibody[0][0] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));
	m_Ibody[1][1] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));
	m_Ibody[2][2] = 1.0 / 6.0 * (m_mass * ((cube->getSize() * cube->getSize())));

	m_IbodyInverse = glm::inverse(m_Ibody);

	m_linearDamping = 0.1f;
	m_angularDamping = 0.75f;
}

RigidBody::RigidBody(RenderSphere* sphere, glm::vec3 pos)
{
	m_sphere = sphere;
	m_mass = 5.0f;
	m_massInverse = 1.0f / m_mass;
	//m_position = glm::vec3(0.0f);
	//m_orientation = glm::mat3(1.0f);
	m_linearVelocity = glm::vec3(0.0f);
	m_angularVelocity = glm::vec3(0.0f);
	m_linearMomentum = glm::vec3(0.0f);
	m_angularMomentum = glm::vec3(0.0f);

	m_position = pos;
	m_orientation = glm::mat3(1.0f);
	m_rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	//m_quatOrientation = glm::quat(0.f, 0.f, 0.f, 0.f);
	//m_orientation = glm::mat3_cast(m_quatOrientation);

	m_Ibody = glm::mat3x3(1.0f);
	m_Ibody[0][0] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));
	m_Ibody[1][1] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));
	m_Ibody[2][2] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));

	m_IbodyInverse = glm::inverse(m_Ibody);

	m_linearDamping = 0.1f;
	m_angularDamping = 0.75f;
}

RigidBody::RigidBody(RenderSphere* sphere, glm::vec3 pos, float mass)
{
	m_sphere = sphere;
	m_mass = mass;
	m_massInverse = 1.0f / m_mass;
	//m_position = glm::vec3(0.0f);
	//m_orientation = glm::mat3(1.0f);
	m_linearVelocity = glm::vec3(0.0f);
	m_angularVelocity = glm::vec3(0.0f);
	m_linearMomentum = glm::vec3(0.0f);
	m_angularMomentum = glm::vec3(0.0f);

	m_position = pos;
	m_orientation = glm::mat3(1.0f);
	m_rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	//m_quatOrientation = glm::quat(0.f, 0.f, 0.f, 0.f);
	//m_orientation = glm::mat3_cast(m_quatOrientation);

	m_Ibody = glm::mat3x3(1.0f);
	m_Ibody[0][0] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));
	m_Ibody[1][1] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));
	m_Ibody[2][2] = 2.0 / 5.0 * (m_mass * ((sphere->getRadius() * sphere->getRadius())));

	m_IbodyInverse = glm::inverse(m_Ibody);

	m_linearDamping = 0.1f;
	m_angularDamping = 0.75f;
}

RigidBody::~RigidBody()
{

}

void RigidBody::integrate(float dt)
{
	//m_force = glm::vec3(0.0f, -9.81f, 0.0f) * m_mass;
	//m_torque = glm::vec3(0.0f);

	//calculate linear and angular momentum
	m_linearMomentum = m_massInverse * m_force;

	//std::cout << "m_torque.x: " << m_torque.x << std::endl;
	glm::mat3 rotMat = glm::mat3_cast(m_rotation);
	m_inverseInertiaTensor = rotMat * m_IbodyInverse * glm::transpose(rotMat);
	m_angularMomentum = m_inverseInertiaTensor * m_torque;

	//update velocities
	m_linearVelocity += m_linearMomentum * dt;
	m_angularVelocity += m_angularMomentum * dt;

	// hier damping eventuell
	//m_linearVelocity *= glm::pow(m_linearDamping, dt);
	//m_angularVelocity *= glm::pow(m_angularDamping, dt);

	m_position += m_linearVelocity * dt;

	//rotation update
	/*m_rotation = m_rotation + dt * glm::quat(0.0f, 0.5f * m_angularVelocity) * m_rotation;
	if (float l = glm::length(m_rotation))
	{
		m_rotation /= l;
	}*/
	float angle = glm::length(m_angularVelocity) * dt;
	if (angle != 0)
	{
		m_rotation = m_rotation + dt * glm::quat(0.0f, 0.5f * m_angularVelocity) * m_rotation;
	}
	m_rotation = glm::normalize(m_rotation);

	//float angle = glm::length(m_angularVelocity) * dt;
	//if (angle != 0)
	//	m_orientation = glm::mat3(glm::rotate(glm::mat4(m_orientation), glm::degrees(angle), m_angularVelocity));

	// clear forces
	m_force = glm::vec3(0.0f);
	m_torque = glm::vec3(0.0f);
}

void RigidBody::applyForce(const glm::vec3& force)
{
	m_force += force;
}

void RigidBody::applyForce(const glm::vec3& force, glm::vec3 position)
{
	glm::vec3 r = position - m_position;

	if (glm::length(r) < 0.0001f)
	{
		//if force acts directly on the center of mass
		m_force += force;
	}
	else
	{
		//if force doesn't act on the center of mass
		r = glm::normalize(r);
		m_force += glm::abs(glm::dot(r, glm::normalize(force)) * force);
	}
}

void RigidBody::applyTorque(const glm::vec3& torque)
{
	m_torque += torque;
}

float RigidBody::getMass()
{
	return m_mass;
}

glm::quat RigidBody::getRotation()
{
	return m_rotation;
}

void RigidBody::setRotation(glm::quat rotation)
{
	m_rotation = rotation;
}

glm::vec3 RigidBody::getPosition()
{
	return m_position;
}

void RigidBody::setPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 RigidBody::getLinearVelocity()
{
	return m_linearVelocity;
}

void RigidBody::setLinearVelocity(glm::vec3 linVel)
{
	m_linearVelocity = linVel;
}