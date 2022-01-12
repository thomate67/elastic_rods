#include "ElasticRods.h"

float GetAngle(const glm::vec3 x, const glm::vec3 y)
{
	float arg = glm::dot(x, y);
	arg = glm::clamp(arg, -1.0f, 1.0f);
	return glm::acos(arg);
}

ElasticRod::ElasticRod()
{

}

ElasticRod::ElasticRod(ElasticRodParameter params, std::vector<glm::vec4> vertices, std::vector<glm::vec3> velocity, std::vector<float> mass, std::vector<float> theta, BoundaryCondition type)
{
	m_params = params;
	m_pos = vertices;
	m_vel = velocity;
	m_mass = mass;
	//m_u0 = u0;							//brauch ich das berhaupt, oder mach ich das eventuell anders
	m_theta = theta;
	m_clampType = type;
	m_restpos = vertices;					//x0-xn with the stroke above them (notation for "in Restposition")

	m_edges.resize(m_pos.size() - 1);
	m_kb.resize(m_pos.size());
	m_t.resize(m_edges.size());
	m_u.resize(m_edges.size());
	m_v.resize(m_edges.size());
	m_m1.resize(m_edges.size());
	m_m2.resize(m_edges.size());
	m_m1Rest.resize(m_edges.size());
	m_m2Rest.resize(m_edges.size());

	m_dEdTheta.resize(m_edges.size());

	m_restEdgeLength.resize(m_edges.size());
	m_restRegionLength.resize(m_edges.size());
	m_voronoiRegion.resize(m_edges.size());
	m_restOmegaprev.resize(m_edges.size());
	m_restOmeganext.resize(m_edges.size());

	m_minusPsiTerm.resize(m_edges.size());
	m_plusPsiTerm.resize(m_edges.size());
	m_equalPsiTerm.resize(m_edges.size());
	m_minusGradKB.resize(m_edges.size());
	m_plusGradKB.resize(m_edges.size());
	m_equalGradKB.resize(m_edges.size());

	m_force.resize(m_pos.size());

	m_constraints.resize(m_edges.size());
	m_constr_diag.resize(m_edges.size());
	m_constr_off_diag.resize(m_edges.size());

	if (m_clampType == BoundaryCondition::BC_FIXED_ONESIDE)
	{
		m_isClamped.insert(0);
	}
	else if (m_clampType == BoundaryCondition::BC_FIXED_TWOSIDE)
	{
		m_isClamped.insert(0);
		m_isClamped.insert(m_edges.size());
	}
	else if (m_clampType == BoundaryCondition::BC_RIGIDBODY)
	{
		m_isClamped.insert(0);
	}

	m_dEdThetaZero = 0.0f;

	setup();
}

ElasticRod::~ElasticRod() = default;

void ElasticRod::setup()
{
	computeEdges();
	computeEdgeLengths();
	computeVoronoiLengths();
	m_rodLength = getRodLength();
	computeTangents();
	updateBishopFrame();
	computeCurvatureBinormal();
	computeMaterialFrame();
	//m_m1Rest = m_m1;				//geht das auch?
	//saves the material frames rotation in Restposition, is needed for rigid-body coupling
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_m1Rest[i] = m_m1[i];
		m_m2Rest[i] = m_m2[i];
	}
	//computeOmega();
	computeMaterialCurvature();
	//updateQuasistaticFrame();
	//updateRod();
}

void ElasticRod::computeEdges()
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_edges[i] = glm::vec3(m_pos[i + 1].x - m_pos[i].x, m_pos[i + 1].y - m_pos[i].y, m_pos[i + 1].z - m_pos[i].z);
	}
}

float ElasticRod::getRodLength()
{
	float rodLength = 0.0f;
	for (int i = 0; i < m_edges.size(); i++)
	{
		rodLength += glm::length(m_edges[i]);
	}
	return rodLength;
}

void ElasticRod::computeEdgeLengths()
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_restEdgeLength[i] = glm::length(m_edges[i]);
	}
}

void ElasticRod::computeVoronoiLengths()
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		if (i == 0)
		{
			m_restRegionLength[i] = m_restEdgeLength[i];
			m_voronoiRegion[i] = 0.5f * m_restRegionLength[i];
		}
		else if (i == m_edges.size() - 1)
		{
			m_restRegionLength[i] = m_restEdgeLength[i];
			m_voronoiRegion[i] = 0.5f * m_restRegionLength[i];
		}
		else
		{
			m_restRegionLength[i] = m_restEdgeLength[i - 1] + m_restEdgeLength[i];
			m_voronoiRegion[i] = 0.5f * m_restRegionLength[i];
		}
	}
}

void ElasticRod::computeTangents()
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_t[i] = glm::normalize(m_edges[i]);
	}
}

void ElasticRod::computeCurvatureBinormal()
{
	m_kb[0] = glm::vec3(0.0f);
	m_kb[m_pos.size() - 1] = glm::vec3(0.0f);

	for (int i = 1; i < m_edges.size(); i++)
	{
		m_kb[i] = (2.0f * glm::cross(m_edges[i - 1], m_edges[i])) / ((m_restEdgeLength[i - 1] * m_restEdgeLength[i]) + glm::dot(m_edges[i - 1], m_edges[i]));
	}
}

glm::vec2 ElasticRod::computeOmega(glm::vec3 & kB, glm::vec3 & m1, glm::vec3 & m2)
{
	return glm::vec2(glm::dot(kB, m2), glm::dot(-(kB), m1));
}

void ElasticRod::computeMaterialCurvature()
{
	m_restOmegaprev[0] = glm::vec2(0.0f);
	m_restOmeganext[0] = glm::vec2(0.0f);

	for (int i = 1; i < m_edges.size(); i++)
	{
		m_restOmegaprev[i] = computeOmega(m_kb[i], m_m1[i - 1], m_m2[i - 1]);
		m_restOmeganext[i] = computeOmega(m_kb[i], m_m1[i], m_m2[i]);
	}
}

void ElasticRod::updateBishopFrame()
{
	//compute Bishop frame for edge 0
	m_u[0] = glm::vec3(m_t[0].z - m_t[0].y, m_t[0].x - m_t[0].z, m_t[0].y - m_t[0].x);
	m_u[0] = glm::normalize(m_u[0]);
	m_v[0] = glm::cross(m_t[0], m_u[0]);
	m_v[0] = glm::normalize(m_v[0]);

	//compute Bishop frame for edge 1...n by parallel transporting m_u along the centerline
	for (int i = 1; i < m_edges.size(); i++)
	{
		m_u[i] = parallelTransport(m_u[i - 1], m_t[i - 1], m_t[i]);
		m_u[i] = glm::normalize(m_u[i]);
		m_v[i] = glm::cross(m_t[i], m_u[i]);
		m_v[i] = glm::normalize(m_v[i]);
	}
}

/*glm::vec3 ElasticRod::parallelTransport(const glm::vec3 & i_u, const glm::vec3 & t0, const glm::vec3 & t1)
{
	glm::vec3 b = glm::cross(t0, t1);

	if (glm::length(b) < 1E-5)
	{
		return i_u;
	}
	else
	{
		b = glm::normalize(b);
		float angle = std::acos(glm::clamp(glm::dot(t0, t1), -1.0f, 1.0f));
		glm::mat3 R = glm::rotate(angle, b);
		return R * i_u;
  
	}
}*/

glm::vec3 ElasticRod::parallelTransport(const glm::vec3& i_u, const glm::vec3& t0, const glm::vec3& t1)
{
	glm::vec3 b = glm::cross(t0, t1);

	if (glm::length(b) == 0.0)
	{
		return i_u;
	}
	else
	{
		b = glm::normalize(b);
		b = b - glm::dot(b, t0) * t0;
		b = glm::normalize(b);
		b = b - glm::dot(b, t1) * t1;
		b = glm::normalize(b);

		glm::vec3 n1 = glm::cross(t0, b);
		glm::vec3 n2 = glm::cross(t1, b);
		glm::vec3 o_u = glm::dot(i_u, t0) * t1 + glm::dot(i_u, n1) * n2 + glm::dot(i_u, b) * b;
		o_u = o_u - glm::dot(o_u, t1) * t1;
		o_u = glm::normalize(o_u);
		return o_u;
	}
}

void ElasticRod::computeMaterialFrame()
{
	for (int i = 0; i < m_edges.size(); i++)
	{
		m_m1[i] = std::cos(m_theta[i]) * m_u[i] + std::sin(m_theta[i]) * m_v[i];
		m_m2[i] = -std::sin(m_theta[i]) * m_u[i] + std::cos(m_theta[i]) * m_v[i];
	}
}

float ElasticRod::computeEnergy()
{
	float energy = 0.0f;
	float mi;
	glm::vec2 omegaij = glm::vec2(0.0f);

	for (int i = 1; i < m_edges.size(); i++)
	{
		// bending energy term
		omegaij = computeOmega(m_kb[i], m_m1[i - 1], m_m2[i - 1]);
		energy += glm::dot((omegaij - m_restOmegaprev[i]), m_params.m_B * (omegaij - m_restOmegaprev[i])) * 0.5 / m_restRegionLength[i];		//muss ich nochmal nachprfen, ob das stimmt

		omegaij = computeOmega(m_kb[i], m_m1[i], m_m2[i]);
		energy += glm::dot((omegaij - m_restOmeganext[i]), m_params.m_B * (omegaij - m_restOmeganext[i])) * 0.5 / m_restRegionLength[i];		//muss ich nochmal nachprfen, ob das stimmt

		//twisting energy term
		mi = (m_theta[i] - m_theta[i - 1]);
		energy += m_params.m_beta * (mi * mi) / m_restRegionLength[i];
	}

	return energy;
}

void ElasticRod::computeCenterlineForces(std::vector<glm::vec3> & forces)
{
	Timer  timer(__func__);

	m_minusGradKB.resize(m_edges.size());
	m_plusGradKB.resize(m_edges.size());
	m_equalGradKB.resize(m_edges.size());
	computeGradientKB();

	m_minusPsiTerm.resize(m_edges.size());
	m_plusPsiTerm.resize(m_edges.size());
	m_equalPsiTerm.resize(m_edges.size());
	computeGradientHolonomyTerms();

	int n = m_edges.size() - 1;
	float dEdTheta = 0.0f;

	glm::vec2 omegakj = glm::vec2(0.0f);
	glm::vec2 omegann = glm::vec2(0.0f);

	glm::vec3 term = glm::vec3(0.0f);
	glm::vec3 gradPsi = glm::vec3(0.0f);

	glm::mat3x2 gradOmega = glm::mat2x3(0.0f);

	for (int i = 0; i < m_pos.size(); i++)
	{
		forces[i] = glm::vec3(0.0f, 0.0f, 0.0f);

		if (m_isClamped.count(i))
		{
			continue;
		}

		for (int k = std::max(i - 1, 1); k < m_edges.size(); k++)
		{
			//compute dE/dx for j = k - 1
			omegakj = computeOmega(m_kb[k], m_m1[k - 1], m_m2[k - 1]);
			computeGradientCurvature(i, k, k - 1, omegakj, gradOmega);

			term = (glm::transpose(gradOmega) * m_params.m_B * (omegakj - m_restOmegaprev[k]));
			//term = (gradOmega * m_params.m_B * (omegakj - m_restOmegaprev[k]));

			//compute dE/dx for j = k
			omegakj = computeOmega(m_kb[k], m_m1[k], m_m2[k]);
			computeGradientCurvature(i, k, k, omegakj, gradOmega);

			term += (glm::transpose(gradOmega) * m_params.m_B * (omegakj - m_restOmeganext[k]));
			//term += (gradOmega * m_params.m_B * (omegakj - m_restOmeganext[k]));

			forces[i] -= term / m_restRegionLength[k];
		}

		if (m_isClamped.size())
		{
			// if ends are clamped  we need to add dE/dTheta * gradient of Psi, since not all twist angles minimize the energy
			omegann = computeOmega(m_kb[n], m_m1[n], m_m2[n]);
			dEdTheta = glm::dot(omegann, (m_J * m_params.m_B) * (omegann - m_restOmeganext[n]));
			dEdTheta += 2 * m_params.m_beta * (m_theta[n] - m_theta[n - 1]);
			dEdTheta /= m_restRegionLength[n];
			//m_dEdThetaZero = dEdTheta;
		
			//std::cout << "dEdTheta centerline: " << dEdTheta << std::endl;
			gradPsi = glm::vec3(0.0f);
			computeGradientPsi(i, n, gradPsi);
			forces[i] += dEdTheta * gradPsi;
		}

		if (glm::dot(forces[i], forces[i]) > m_params.m_maxForce * m_params.m_maxForce)
		{
			forces[i] = glm::normalize(forces[i]);
			forces[i] *= m_params.m_maxForce;
		}
	}
}

void ElasticRod::computedEdThetaAndHessian(std::vector<float> & theta, std::vector<float> & dEdTheta, std::vector<float> & Hjj, std::vector<float> & Hjjminus1, std::vector<float> & Hjjplus1)
{
	dEdTheta.resize(m_edges.size());
	Hjj.resize(m_edges.size());
	Hjjminus1.resize(m_edges.size());
	Hjjplus1.resize(m_edges.size());

	std::fill(dEdTheta.begin(), dEdTheta.end(), 0);
	std::fill(Hjj.begin(), Hjj.end(), 0);
	std::fill(Hjjminus1.begin(), Hjjminus1.end(), 0);
	std::fill(Hjjplus1.begin(), Hjjplus1.end(), 0);

	glm::mat2 JB = m_J * m_params.m_B;
	float term = 0.0f;
	float hjj = 0.0f;
	glm::vec2 omegaij = glm::vec2(0.0f);

	for (int i = 0; i < m_edges.size(); i++)
	{
		dEdTheta[i] = Hjjminus1[i] = Hjj[i] = Hjjplus1[i] = 0.0f;
		term = 0.0f;

		if (i > 0)
		{
			omegaij = computeOmega(m_kb[i], m_m1[i], m_m2[i]);
			//compute dE/dtheta
			term = glm::dot(omegaij, JB * (omegaij - m_restOmeganext[i]));
			term += 2 * m_params.m_beta * (theta[i] - theta[i - 1]);
			term /= m_restRegionLength[i];
			dEdTheta[i] += term;

			//compute Hessian
			Hjjminus1[i] = -2 * m_params.m_beta / m_restRegionLength[i];

			hjj = 2 * m_params.m_beta;
			hjj += glm::dot(omegaij, glm::transpose(m_J) * m_params.m_B * m_J * omegaij);
			hjj -= glm::dot(omegaij, m_params.m_B * (omegaij - m_restOmeganext[i]));
			hjj /= m_restRegionLength[i];
			Hjj[i] = hjj;
		}

		if (i < m_edges.size() - 1)
		{
			omegaij = computeOmega(m_kb[i + 1], m_m1[i], m_m2[i]);
			//compute dE/dtheta
			term = glm::dot(omegaij, JB * (omegaij - m_restOmegaprev[i + 1]));
			term -= 2 * m_params.m_beta * (theta[i + 1] - theta[i]);
			term /= m_restRegionLength[i + 1];
			dEdTheta[i] += term;

			//compute Hessian
			Hjjplus1[i] = -2 * m_params.m_beta / m_restRegionLength[i + 1];

			hjj = 2 * m_params.m_beta;
			hjj += glm::dot(omegaij, glm::transpose(m_J) * m_params.m_B * m_J * omegaij);
			hjj -= glm::dot(omegaij, m_params.m_B * (omegaij - m_restOmegaprev[i + 1]));
			hjj /= m_restRegionLength[i + 1];
			Hjj[i] = hjj;
		}
	}
}

void ElasticRod::computeGradientHolonomyTerms()
{
	m_minusPsiTerm[0] = m_plusPsiTerm[0] = m_equalPsiTerm[0] = glm::vec3(0.0f);

	for (int i = 1; i < m_edges.size(); i++)
	{
		m_minusPsiTerm[i] = 0.5f * m_kb[i] / m_restEdgeLength[i - 1];
		m_plusPsiTerm[i] = -0.5f * m_kb[i] / m_restEdgeLength[i];
		m_equalPsiTerm[i] = -(m_minusPsiTerm[i] + m_plusPsiTerm[i]);
	}
}

void ElasticRod::computeGradientPsi(int i, int j, glm::vec3 & gradPsi)
{
	gradPsi = glm::vec3(0.0f);

	if (j >= (i - 1) && i > 1 && (i - 1) < m_plusPsiTerm.size())
	{
		gradPsi += m_plusPsiTerm[i - 1];
	}

	if (j >= i && i < m_equalPsiTerm.size())
	{
		gradPsi += m_equalPsiTerm[i];
	}

	if (j >= (i + 1) && (i + 1) < m_minusPsiTerm.size())
	{
		gradPsi += m_minusPsiTerm[i + 1];
	}
}

void ElasticRod::computeGradientKB()
{
	std::vector<glm::mat3x3> edgeMatrix(m_edges.size());
	for (int i = 0; i < m_edges.size(); i++)
	{
		edgeMatrix[i] = MathFunctions::skew_symmetric(m_edges[i]);
	}

	m_minusGradKB[0] = m_plusGradKB[0] = m_equalGradKB[0] = glm::mat3x3(0.0f);

	for (int i = 1; i < m_edges.size(); i++)
	{
		m_minusGradKB[i] = (2.0f * edgeMatrix[i] + glm::outerProduct(m_kb[i], m_edges[i])) / (m_restEdgeLength[i - 1] * m_restEdgeLength[i] + glm::dot(m_edges[i - 1], m_edges[i]));
		m_plusGradKB[i] = (2.0f * edgeMatrix[i - 1] - glm::outerProduct(m_kb[i], m_edges[i - 1])) / (m_restEdgeLength[i - 1] * m_restEdgeLength[i] + glm::dot(m_edges[i - 1], m_edges[i]));
		//m_minusGradKB[i] = (2.0f * edgeMatrix[i-1] + glm::outerProduct(m_kb[i], m_edges[i-1])) / (m_restEdgeLength[i - 1] * m_restEdgeLength[i] + glm::dot(m_edges[i - 1], m_edges[i]));
		//m_plusGradKB[i] = (2.0f * edgeMatrix[i] - glm::outerProduct(m_kb[i], m_edges[i])) / (m_restEdgeLength[i - 1] * m_restEdgeLength[i] + glm::dot(m_edges[i - 1], m_edges[i]));
		m_equalGradKB[i] = -(m_minusGradKB[i] + m_plusGradKB[i]);
	}
}

void ElasticRod::computeGradientCurvature(int i, int k, int j, glm::vec2 & omegakj, glm::mat3x2& gradOmega)
{
	gradOmega = glm::mat3x2(0.0f);

	if (k < i + 2)
	{
		gradOmega[0][0] = m_m2[j][0];
		gradOmega[1][0] = m_m2[j][1];
		gradOmega[2][0] = m_m2[j][2];

		gradOmega[0][1] = -m_m1[j][0];
		gradOmega[1][1] = -m_m1[j][1];
		gradOmega[2][1] = -m_m1[j][2];

		if (k == (i - 1))
		{
			//gradOmega = glm::transpose(gradOmega) * m_plusGradKB[k];
			gradOmega = gradOmega * m_plusGradKB[k];
		}
		else if (k == i)
		{
			//gradOmega = glm::transpose(gradOmega) * m_equalGradKB[k];
			gradOmega = gradOmega * m_equalGradKB[k];
		}
		else if (k == i + 1)
		{
			//gradOmega = glm::transpose(gradOmega) * m_minusGradKB[k];
			gradOmega = gradOmega * m_minusGradKB[k];
		}
	}

	glm::vec3 gradPsi = glm::vec3(0.0f);
	computeGradientPsi(i, j, gradPsi);
	gradOmega -= m_J * glm::outerProduct(omegakj, gradPsi);
}

void ElasticRod::updateRod()
{
	computeEdges();
	computeTangents();
	computeCurvatureBinormal();
	updateBishopFrame();
	updateQuasistaticFrame();
	computeMaterialFrame();
	//computeMaterialCurvature();

}

void ElasticRod::updateQuasistaticFrame()
{
	//kommt spter in die params
	int NewtonMaxIter = 10;
	double NewtonTolerance = 1e-12;
	//double NewtonTolerance = 0.2;

	//std::vector<float> dEdTheta;
	std::vector<float> Hjj;
	std::vector<float> Hjjminus1;
	std::vector<float> Hjjplus1;

	//dEdTheta.resize(m_edges.size());
	Hjj.resize(m_edges.size());
	Hjjminus1.resize(m_edges.size());
	Hjjplus1.resize(m_edges.size());

	std::vector<float> thetas;
	thetas.resize(m_edges.size());

	for (int i = 0; i < m_edges.size(); i++)
	{
		thetas[i] = m_theta[i];
	}

	int t;
	for (t = 0; t < NewtonMaxIter; t++)
	{
		computedEdThetaAndHessian(thetas, m_dEdTheta, Hjj, Hjjminus1, Hjjplus1);

		switch (m_clampType)
		{
		case BoundaryCondition::BC_FREE:
			break;
		case BoundaryCondition::BC_FIXED_ONESIDE:
			m_dEdTheta[0] = 0.0f;
			Hjj[0] = 1.0f;
			Hjjplus1[0] = 0.0f;
			break;
		case BoundaryCondition::BC_FIXED_TWOSIDE:
			m_dEdTheta[0] = 0.0f;
			m_dEdTheta[m_edges.size() - 1] = 0.0f;
			Hjj[0] = 1.0f;
			Hjj[m_edges.size() - 1] = 1.0f;
			Hjjplus1[0] = 0.0f;
			Hjjminus1[m_edges.size() - 1] = 0.0f;
			break;
		case BoundaryCondition::BC_RIGIDBODY:
			m_dEdTheta[0] = 0.0f;
			Hjj[0] = 1.0f;
			Hjjplus1[0] = 0.0f;
			break;
		default:
			break;
		}

		float sum = 0.0f;
		for (int n = 0; n < m_dEdTheta.size(); n++)
		{
			sum += m_dEdTheta[n] * m_dEdTheta[n];
			//std::cout << m_dEdTheta[n] << std::endl;
		}
		float dEnorm = sqrt(sum);
		//std::cout << "Norm od dEdTheta: " << dEnorm << std::endl;

		if (dEnorm < NewtonTolerance)
		{
			std::cout << "Newton's method converges in " << t << " steps." << std::endl;
			
			break;
		}
		MathFunctions::tridiagonalSolver(Hjjminus1, Hjj, Hjjplus1, m_dEdTheta);
		for (int n = 0; n < thetas.size(); n++)
		{
			thetas[n] -= m_dEdTheta[n];
		}
	}

	if (t < NewtonMaxIter)
	{
		for (int i = 0; i < m_edges.size(); i++)
		{
			m_theta[i] = thetas[i];
			computeMaterialFrame();
		}
	}
}

void ElasticRod::simulate(float dt)
{
	if (m_thetaOverTime)
	{
		m_theta[m_theta.size() - 1] += dt * (0.01f / M_PI);
	}
	
	//m_theta[m_theta.size() - 1] = dt * 100.0f / M_PI;
	//m_theta[0] += 0.1f / M_PI;
	//updateRod();
	//computeMaterialFrame();
	//updateQuasistaticFrame();

	for (int i = 0; i < m_pos.size(); i++)
	{
		m_force[i] = glm::vec3(0.0f);
		//std::cout << "Kraft: " << m_force[i].x << " ; " << m_force[i].y << " ; " << m_force[i].z << std::endl;
	}
	
	computeCenterlineForces(m_force);
	if (m_externalForcesEnabled)
	{
		addExternalForces(m_force);
	}
	
	std::vector<glm::vec4> prevPos = m_pos;

	glm::vec3 damping_force = glm::vec3(0.0f);

	for (int i = 0; i < m_pos.size(); i++)
	{
		if (m_isClamped.count(i))
		{
			continue;
		}

		
		m_vel[i] += dt * m_force[i] / m_mass[i];
		
		
		if (m_dampingEnabled)
		{
			damping_force = 0.001f * m_vel[i];
			m_vel[i] -= damping_force;
		}
		m_pos[i] += glm::vec4(glm::vec3(dt * m_vel[i]), 0.0f);
	}

	//distancefield collisions methode
	//std::vector<glm::vec4> oldPos = m_pos;
	dfCollision(prevPos, dt);

	enforceInextensibility();

	for (int i = 0; i < m_pos.size(); i++)
	{
		//	velocity correction via Verlet scheme
		//glm::vec3 old_vel = m_vel[i];
		//m_vel[i] = (glm::vec3(m_pos[i].x, m_pos[i].y, m_pos[i].z) - glm::vec3(prevPos[i].x, prevPos[i].y, prevPos[i].z)) / dt;
		//m_vel[i] = glm::mix(old_vel, m_vel[i], 0.3f);
		m_vel[i] = (glm::vec3(m_pos[i]) - glm::vec3(prevPos[i])) / dt;
	}

	updateRod();
}

void ElasticRod::addExternalForces(std::vector<glm::vec3>& forces)
{
	for (int i = 0; i < m_pos.size(); i++)
	{
		if (m_gravityEnabled)
		{
			forces[i] += m_gravity * m_mass[i];
		}
		if (m_windEnabled)
		{
			//TODO: Wind muss überhaupt noch implimentiert werden!
			// aber müsste da nicht einfach ein vec3 reichen, der in x bläst 
			//forces[i] += m_wind *kp ob da noch was hin muss gedöns;
		}
	}
}

void ElasticRod::enforceInextensibility()
{
	glm::vec3 e = glm::vec3(0.0f);
	float l;
	float l1;
	float l2;

	float currRodLength = getRodLength();

	for (int i = 0; i < m_pos.size() - 1; i++)
	{
		bool clamped_i = m_isClamped.count(i) > 0;
		bool clamped_i1 = m_isClamped.count(i + 1) > 0;

		e = glm::vec3(m_pos[i + 1] - m_pos[i]);
		l = 1 - 2 * m_restEdgeLength[i] * m_restEdgeLength[i] / (m_restEdgeLength[i] * m_restEdgeLength[i] + glm::dot(e, e));
		//l = glm::dot(e, e) - glm::dot(m_restEdgeLength[i], m_restEdgeLength[i]);

		if (clamped_i)
		{
			l1 = 0;
			l2 = -l;
		}
		else if (clamped_i1)
		{
			l1 = l;
			l2 = 0;
		}
		else
		{
			l1 = m_mass[i + 1] / (m_mass[i] + m_mass[i + 1]) * l;
			l2 = -m_mass[i] / (m_mass[i] + m_mass[i + 1]) * l;
			//l1 = l;
			//l2 = -l;
		}

		m_pos[i] += glm::vec4((l1 * e), 0.0f);
		m_pos[i + 1] += glm::vec4(l2 * e, 0.0f);
	}
}

void ElasticRod::dfCollision(std::vector<glm::vec4> old_pos, float dt)
{
	//float d1 = map(old_pos) - thickness / 2;			//thickness/2 war für particle, sollte high_res rendering noch klappen, dann muss ich cone_radius/2 da machen
	//float d2 = map(new_pos) - thickness / 2;			//m_rodThickness = thickness; ->  - m_rodThickness / 2;
	for (int i = 0; i < m_pos.size(); i++)
	{
		if (m_cubeCollision)
		{
			//cube
			float d1 = Distancefield::sdBox(glm::vec3(old_pos[i]), glm::vec3(m_collisionCubeSize, m_collisionCubeSize, m_collisionCubeSize), m_cubeCollisionMat) - m_rodThickness;
			float d2 = Distancefield::sdBox(glm::vec3(m_pos[i]), glm::vec3(m_collisionCubeSize, m_collisionCubeSize, m_collisionCubeSize), m_cubeCollisionMat) - m_rodThickness;
			glm::vec3 n = Distancefield::sdBoxGrad(glm::vec3(m_pos[i]), glm::vec3(m_collisionCubeSize, m_collisionCubeSize, m_collisionCubeSize), m_cubeCollisionMat);

			if (d2 < 0 && d1 >= 0) 
			{
				m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
				glm::vec3 new_pos = glm::vec3(old_pos[i]) + dt * d1 / (d1 - d2) * m_vel[i];
				m_pos[i] = glm::vec4(new_pos, 1.0f);
				//m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
			}
			else if (d1 < 0 && d2 < 0) 
			{
				if (glm::dot(n, m_vel[i]) < 0)
					m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
				m_pos[i] -= glm::vec4(d1 * n, 0.0f);
			}
		}
		//sphere
		if (m_sphereCollision)
		{
			float d1 = Distancefield::sdSphere(glm::vec3(old_pos[i]), m_collisionSphereRadius, m_sphereCollisionMat) - m_rodThickness;
			float d2 = Distancefield::sdSphere(glm::vec3(m_pos[i]), m_collisionSphereRadius, m_sphereCollisionMat) - m_rodThickness;
			glm::vec3 n = Distancefield::sdSphereGrad(glm::vec3(m_pos[i]), m_collisionSphereRadius, m_sphereCollisionMat);

			if (d2 < 0 && d1 >= 0) 
			{
				glm::vec3 new_pos = glm::vec3(old_pos[i]) + dt * d1 / (d1 - d2) * m_vel[i];
				m_pos[i] = glm::vec4(new_pos, 1.0f);
				m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
			}
			else if (d1 < 0 && d2 < 0) 
			{
				if (glm::dot(n, m_vel[i]) < 0)
					m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
				m_pos[i] -= glm::vec4(d1 * n, 0.0f);
			}
		}

		if (m_headCollision)
		{
			float d1 = Distancefield::sdSphere(glm::vec3(old_pos[i]), m_collisionHeadRadius, m_headCollisionMat) - m_rodThickness;
			float d2 = Distancefield::sdSphere(glm::vec3(m_pos[i]), m_collisionHeadRadius, m_headCollisionMat) - m_rodThickness;
			glm::vec3 n = Distancefield::sdSphereGrad(glm::vec3(m_pos[i]), m_collisionHeadRadius, m_headCollisionMat);

			if (d2 < 0 && d1 >= 0)
			{
				glm::vec3 new_pos = glm::vec3(old_pos[i]) + dt * d1 / (d1 - d2) * m_vel[i];
				m_pos[i] = glm::vec4(new_pos, 1.0f);
				m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
			}
			else if (d1 < 0 && d2 < 0)
			{
				if (glm::dot(n, m_vel[i]) < 0)
					m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
				m_pos[i] -= glm::vec4(d1 * n, 0.0f);
			}
		}
		
		if (m_planeCollision)
		{
			//plane
			float d1 = Distancefield::sdPlane(glm::vec3(old_pos[i]), m_planeCollisionMat) - m_rodThickness;
			float d2 = Distancefield::sdPlane(glm::vec3(m_pos[i]), m_planeCollisionMat) - m_rodThickness;
			glm::vec3 n = Distancefield::sdPlaneGrad(glm::vec3(m_pos[i]), m_planeCollisionMat);

			if (d2 < 0 && d1 >= 0) 
			{
				glm::vec3 new_pos = glm::vec3(old_pos[i]) + dt * d1 / (d1 - d2) * m_vel[i];
				m_pos[i] = glm::vec4(new_pos, 1.0f);
				m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
			}
			else if (d1 < 0 && d2 < 0) 
			{
				if (glm::dot(n, m_vel[i]) < 0)
					m_vel[i] -= 1.99f * glm::dot(m_vel[i], n) * n;
				m_pos[i] -= glm::vec4(d1 * n, 0.0f);
			}
		}
	}
}

void ElasticRod::move(GLFWwindow* window)
{
	bool m_mousePressed = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		m_pos[(m_pos.size() - 1) / 2] = glm::vec4(m_pos[(m_pos.size() - 1) / 2].x, m_pos[(m_pos.size() - 1) / 2].y - 0.025f, m_pos[(m_pos.size() - 1) / 2].z, 1.0f);
	}

	//if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	//{
	//	m_pos[0] += glm::vec4(-0.001f, 0.0f, 0.0f, 0.0f);
	//	m_pos[m_pos.size() - 1] += glm::vec4(0.001f, 0.0f, 0.0f, 0.0f);
	//}
	//
	//if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	//{
	//	m_pos[0] += glm::vec4(0.001f, 0.0f, 0.0f, 0.0f);
	//	m_pos[m_pos.size() - 1] += glm::vec4(-0.001f, 0.0f, 0.0f, 0.0f);
	//}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		m_pos[0] += glm::vec4(0.001f, 0.0f, 0.0f, 0.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		m_pos[0] += glm::vec4(-0.001f, 0.0f, 0.0f, 0.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		m_pos[0] += glm::vec4(0.001f, 0.0f, 0.0f, 0.0f);
	}
	
	//enforceInextensibility();
	updateRod();
}

std::vector<glm::vec4> ElasticRod::getVertices()
{
	return m_pos;
}

std::vector<glm::vec3> ElasticRod::getEdges()
{
	return m_edges;
}

std::vector<glm::vec3> ElasticRod::getBishopT()
{
	return m_t;
}

std::vector<glm::vec3> ElasticRod::getBishopU()
{
	return m_u;
}

std::vector<glm::vec3> ElasticRod::getBishopV()
{
	return m_v;
}

std::vector<glm::vec3> ElasticRod::getMaterial1()
{
	return m_m1;
}

std::vector<glm::vec3> ElasticRod::getMaterial2()
{
	return m_m2;
}

glm::vec3 ElasticRod::getM1Frame(int i)
{
	return m_m1[i];
}

void ElasticRod::setM1Frame(int i, glm::vec3 newM1)
{
	m_m1[i] = newM1;
}

glm::vec3 ElasticRod::getM2Frame(int i)
{
	return m_m2[i];
}

void ElasticRod::setM2Frame(int i, glm::vec3 newM2)
{
	m_m2[i] = newM2;
}

float ElasticRod::getdEdTheta(int i)
{
	return m_dEdTheta[i];
}

glm::vec3 ElasticRod::getM1RestFrame(int i)
{
	return m_m1Rest[i];
}

glm::vec3 ElasticRod::getM2RestFrame(int i)
{
	return m_m2Rest[i];
}

void ElasticRod::setU0(glm::vec3 u0)
{
	m_u0 = u0;
}

glm::vec3 ElasticRod::getU0()
{
	return m_u0;
}

glm::vec4 ElasticRod::getEndPos()
{
	return m_pos[m_pos.size() - 1];
}

void ElasticRod::setEndPos(glm::vec4 pos)
{
	m_pos[m_pos.size() - 1] = pos;
}

glm::vec3 ElasticRod::getEndVel()
{
	return m_vel[m_pos.size() - 1];
}

void ElasticRod::setEndVel(glm::vec3 vel)
{
	m_vel[m_pos.size() - 1] = vel;
}

void ElasticRod::setLastTheta(float angle)
{
	m_theta[m_theta.size() - 1] = angle;
}

void ElasticRod::setLastTheta(bool status)
{
	m_thetaOverTime = status;
}

float ElasticRod::getLastTheta()
{
	return m_theta[m_theta.size() - 1];
}

void ElasticRod::setExternalForceStatus(bool external, bool gravity, bool wind, bool damping)
{
	m_externalForcesEnabled = external;
	m_gravityEnabled = gravity;
	m_windEnabled = wind;
	m_dampingEnabled = damping;
}

void ElasticRod::setPlaneCollision(bool status, float position, float thickness)
{
	m_planeCollision = status;
	m_rodThickness = thickness;
	//m_planePositionY = position;
	m_planeCollisionMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, position, 0.0f));
}

void ElasticRod::setCubeCollision(bool status, glm::vec3 position, float size, float thickness)
{
	m_cubeCollision = status;
	m_rodThickness = thickness;
	m_collisionCubeSize = size;
	m_cubeCollisionMat = glm::translate(glm::mat4(1.0f), position);
}

void ElasticRod::setSphereCollision(bool status, glm::vec3 position, float radius, float thickness)
{
	m_sphereCollision = status;
	m_rodThickness = thickness;
	m_collisionSphereRadius = radius;
	m_sphereCollisionMat = glm::translate(glm::mat4(1.0f), position);
}

void ElasticRod::setHeadCollision(bool status, glm::vec3 position, float radius, float thickness)
{
	m_headCollision = status;
	m_rodThickness = thickness;
	m_collisionHeadRadius = radius;
	m_headCollisionMat = glm::translate(glm::mat4(1.0f), position);
}