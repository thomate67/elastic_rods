#pragma once

#include <src/libraries/Utility/Definitions.h>
#include <src/libraries/Math/MathFunctions.h>
#include <src/libraries/Math/Distancefield.h>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>

extern std::ofstream log_file;

struct Timer {
	double start_time;
	std::string label;

	Timer(std::string s) {
		start_time = glfwGetTime();
		label = s;
	}

	~Timer() {
		double end_time = glfwGetTime();
		log_file << label << ":" << (end_time - start_time) * 1000 << " ms" << std::endl;
	}
};

struct ElasticRodParameter
{
	ElasticRodParameter(float bendStiffness = 1.0,
		float twistStiffness = 1.0,
		int maxElasticForce = 1000) :
		m_alpha(bendStiffness),
		m_beta(twistStiffness),
		m_maxForce(maxElasticForce)
	{
		setBendStiffness(bendStiffness);
	}

	inline void setBendStiffness(float& bendStiffness)
	{
		m_alpha = bendStiffness;
		m_B *= m_alpha;									//matrix B = [ 1 0 ] = [m_alpha         0    ]
	    												//			 [ 0 1 ]   [   0         m_alpha ]
	}

	inline void setTwistStiffness(float& twistStiffness)
	{
		m_beta = twistStiffness;
	}

	float m_alpha;		//bending stiffness, material properties measured and constant throughout simulation
	float m_beta;		//twisting stiffness, material properties measured and constant throughout simulation
	int m_maxForce;		//limitation for force, when e[i] ~ -e[i - 1], ||kb|| goes to inf
	glm::mat2 m_B = glm::mat2(1.0f);		//a symmetric positive definite identity 2x2 matrix
};

/**
* @brief describes the boundary condition on the material frame
* @detail the type determines if an edge and which is stored in m_isClamped
* @detail this is important for the computation of the force on the centerline and the quasistatic update of the material frame
* @param BC_FREE, stressfree ends = no boundary conditions
* @param BC_FIXED_ONESIDE, rod is clamped at one end, assigning material frame for j=0
* @param BC_FIXED_TWOSIDE, rod is clamped at both ends, assigning material frame for j=0 and j=n
* @param BC_RIGIDBODY, rod is connected to a rigid body at one end
*/
enum class BoundaryCondition
{
	BC_FREE = 0,
	BC_FIXED_ONESIDE = 1,
	BC_FIXED_TWOSIDE = 2,
	BC_RIGIDBODY = 3
};

class ElasticRod
{
public:
	/**
	* @brief initilazies parameter for Elastic Rod
	* @detail sets initial position, velocity, angles and vector sizes and calls setup()
	* @see setup()
	*/
	ElasticRod();
	ElasticRod(ElasticRodParameter params, std::vector<glm::vec4> vertices, std::vector<glm::vec3> velocity, std::vector<float> mass, std::vector<float> theta, BoundaryCondition type);
	
	/**
	* @brief default deconstructor
	*/
	virtual ~ElasticRod();

	/**
	* @brief initilazies Elastic Rod for first iteration 
	* @detail calls all necessary functions to calculate Rod in rest shape to begin simulation
	* @see computeEdges(), computeEdgeLengths(), computeVoronoiLengths(), computeTangents(), updateBishopFrame(), computeMaterialFrame(), computeCurvatureBinormal(), computeMaterialCurvature()
	*/
	void setup();

	/**
	* @brief deriving the forces on the rod's centerline
	* @detail energy depends on the x_i variables - both directly and indirectly by considering the effect that moving a vertex has on the rod's material frame
	* @see "Bergou et. al. Discrete Elastic Rods" Section 7.1
	*/
	void computeCenterlineForces(std::vector<glm::vec3>& forces); //computes Forces on centerline 7.1

	/**
	* @brief updates the parameters for rod calculation
	* @detail uses updated vertices and thetas to recalculate edges, frames etc.
	*/
	void updateRod();

	float getRodLength();

	std::vector<glm::vec4> getVertices();						//returns vertex position
	std::vector<glm::vec3> getEdges();							//returns axis for Bishop Frame for Debug purpose
	std::vector<glm::vec3> getBishopT();						//returns t axis for Bishop Frame for Debug purpose 
	std::vector<glm::vec3> getBishopU();						//returns u axis for Bishop Frame for Debug purpose 
	std::vector<glm::vec3> getBishopV();						//returns v axis for Bishop Frame for Debug purpose 
	std::vector<glm::vec3> getMaterial1();						//returns m1 axis for Material Frame for Debug purpose 
	std::vector<glm::vec3> getMaterial2();						//returns m2 axis for Material Frame for Debug purpose 

	void setLastTheta(float angle);
	void setLastTheta(bool status);
	float getLastTheta();
	void setExternalForceStatus(bool external, bool gravity, bool wind, bool damping);
	void setPlaneCollision(bool status, float position, float thickness);
	void setCubeCollision(bool status, glm::vec3 position, float size, float thickness);
	void setSphereCollision(bool status, glm::vec3 position, float radius, float thickness);
	void setHeadCollision(bool status, glm::vec3 position, float radius, float thickness);
	glm::vec3 getM1Frame(int i);							//returns material frame m1 at position i
	void setM1Frame(int i, glm::vec3 newM1);				//sets material frame m1 at position i to input value
	glm::vec3 getM2Frame(int i);							//returns material frame m2 at position i
	void setM2Frame(int i, glm::vec3 newM2);				//sets material frame m2 at position i to input value
	float getdEdTheta(int i);	
	
	glm::vec3 getM1RestFrame(int i);						//returns material frame m1Rest at position i
	glm::vec3 getM2RestFrame(int i);						//returns material frame m2Rest at position i

	void setU0(glm::vec3 u0);
	glm::vec3 getU0();

	glm::vec4 getEndPos();
	void setEndPos(glm::vec4 pos);
	glm::vec3 getEndVel();
	void setEndVel(glm::vec3 vel);

	void simulate(float dt);

	void move(GLFWwindow* window);

private:
	/**
	* @brief computes n edges e[i] for the centerline out of n + 1 vertices
	*/
	void computeEdges();			

	/**
	* @brief Computes rest edge lengths
	* @detail m_restEdgeLength[i], contains the length of each edge of the centerline in rest state
	* @see "Bergou et. al. Discrete Elastic Rods" 4.2 Pointwise vs. integrated quantities
	*/
	void computeEdgeLengths();

	/**
	* @brief Computes Voronoi region Di
	* @detail m_voronoiRegionLength[i], contains the Voronoi region Di = li/ 2; li = |e[i - 1]| + |e[i]|
	* @see "Bergou et. al. Discrete Elastic Rods" 4.2 Pointwise vs. integrated quantities
	*/
	void computeVoronoiLengths();

	/**
	* @brief computes unit tangent vector by normalizing edge[i]
	* @see "Bergou et. al. Discrete Elastic Rods" 4.2 Discrete framed curves
	*/
	void computeTangents();	//kp ob ich das wirklich brauche oder einfach normalize(edge[i]) benutze

	/**
	* @brief computes the curvature binormal kB[i]
	* @detail defines rotation from e[i-1] to e[i]
	* @note kB[0] = vec3(0.0)
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (1)
	*/
	void computeCurvatureBinormal();

	/**
	* @brief computes the material curvature omega
	* @param kB, curvature binormal
	* @param m1, first material frame axis m1(normal)
	* @param m2, second material frame axis m2(binormal)
	* @return material curvature o_omega
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (2)
	*/
	glm::vec2 computeOmega(glm::vec3& kb, glm::vec3& m1, glm::vec3& m2);

	/**1
	* @brief computes the material curvature omega for j {i-1, i} this is important for the Bending energy
	* @detail as the Formula in the Paper states, j has a range from i - 1 to i
	* @detail for j = i-1 the m1 and m2 axis from the previous edge is used
	* @detail for j = i the m1 and m2 axis of the current edge is used
	* @detail material curvature of previous edge is stored in m_restOmegaprev
	* @detail material curvature of current edge is stored in m_restOmeganext
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (2) and Formula (3)
	*/
	void computeMaterialCurvature();

	/**
	* @brief computes the Bishop frames {t, u, v} for each edge
	* @detail tangents are given by computeTangents()
	* @see computeTangents()
	* @detail first u0 and v0 for edge0 are computed, thereafter ui and vi are computed using the parallel transport method
	* @see parallelTransport(i_u, t0, t1, o_u)
	* @see "bergou et. al Discrete Elastic Rods" 4.2.2 Bishop frame
	*/
	void updateBishopFrame();

	/**
	* @brief transporting a unit vector u0 along the rod
	* @param i_u, u axis of the previous edge
	* @param t0, tangent of the previous edge
	* @param t1, tangent of the current edge
	* @param o_u, parallel transported u axis to the current edge
	* @return o_u u axis of the current edge
	* @see "Bergou et. al Discrete Elastic Rods" 4.2.2 Discrete parallel transport
	*/
	glm::vec3 parallelTransport(const glm::vec3& i_u, const glm::vec3& t0, const glm::vec3& t1);

	/**
	* @brief computes the Material frames {t, m1, m2} for each edge
	* @detail uses the u and v axis of the Bishop frames to compute m1 and m2 using angle theta
	* @see "Bergou et. al. Discrete Elastic Rods" 4.2.2 Material frame representation
	*/
	void computeMaterialFrame();				// {t, m1, m2}

	/**
	* @brief computes the bending and twisting Energy of the discrete rod
	* @return returns the total enery of the discrete rod
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (3)
	*/
	float computeEnergy();

	/**
	* @brief transporting a unit vector u0 along the rod
	* @detail computes the gradient and Hessian of the energy with respect to the theta^j variables
	* @param theta, current theta angle of the elastic rod
	* @param dEdTheta, vector that contains the Energy with respect to angle theta for the material frame
	* @param Hjj, Hessian H, obtained by differentiating the dEdTheta formula (7) with respect to theta^j
	* @param Hjjminus1, Hessian H, obtained by differentiating the dEdTheta formula (7) with respect to theta^j-1
	* @return Hjjplus1, Hessian H, obtained by differentiating the dEdTheta formula (7) with respect to theta^j+1
	* @see "Bergou et. al Discrete Elastic Rods" Formula (7)
	*/
	void computedEdThetaAndHessian(std::vector<float>& theta, std::vector<float>& dEdTheta, std::vector<float>& Hjj, std::vector<float>& Hjjminus1, std::vector<float>& Hjjplus1);						// Formel (7)

	/**
	* @brief computes the terms for determining the Holonomy gradient
	* @detail computes gradients for: grad_i-1 psi_i; grad_i+1 psi_i; grad_i psi_i
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (9)
	*/
	void computeGradientHolonomyTerms();

	/**
	* @brief computes the gradient Psi
	* @detail gradient of the angle Psi (holonomy scalar) with respect to vertex positions
	* @detail sum of Formula (9), this sum will have at most three non-zero terms
	* @param i, vertex position
	* @param j, range k = 1 -> j, determines Psi
	* @param gradPsi, vec3 that stores the gradient of Psi
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (10)
	*/
	void computeGradientPsi(int i, int j, glm::vec3& gradPsi);

	/**
	* @brief computes the the gradient of the curvature binormal
	* @detail uses a 3x3 skew-symmetric matrix [e] action on 3-vectors x by [e] dot x = e cross x
	* @detail computes matrices for: grad_i-1 (kb)_i; grad_i+1 (kb)_i; grad_i (kb)_i
	* @see "Bergou et. al. Discrete Elastic Rods" Section 7.1 -> Special case page 7
	*/
	void computeGradientKB();

	/**
	* @brief computes gradient of the material-frame curvature
	* @param i, index of the vertex
	* @param k, range j = k-1 -> k 
	* @param j, edge j
	* @param omegakj, material curvature k for edge j
	* @param gradOmega, 3x2 matrix 
	* @see "Bergou et. al. Discrete Elastic Rods" Formula (11)
	*/
	void computeGradientCurvature(int i, int k, int j, glm::vec2& omegakj, glm::mat3x2& gradOmega);			// Formula (11)

	/**
	* @brief quasistatic update of the material frame
	* @detail Newton method to minimize the elastic energy with respect to the material frame
	* @detail uses both the gradient and Hessian of the energy with respect to theta^j
	* @detail updates theta values to update material frame axis m1 and m2
	* @see computedEdThetaAndHessian() 
	* @see "Bergou et. al. Discrete Elastic Rods" Section 5
	*/
	void updateQuasistaticFrame();

	void addExternalForces(std::vector<glm::vec3>& forces);

	void enforceInextensibility();

	void dfCollision(std::vector<glm::vec4> old_pos, float dt);


	/*******************parameter******************/
	ElasticRodParameter m_params;

	std::vector<glm::vec3> m_vel;				// vertices = n + 1, position of the centerline
	std::vector<glm::vec4> m_pos;				// velocity of vertices
	std::vector<float> m_mass;					// mass of the vertices
	glm::vec3 m_u0;								// u vector for edge 0 for parallel transport
	std::vector<float> m_theta;					// theta angles to rotate Bishop frame into Material frame
	std::vector<float> m_dEdTheta;
	std::set<unsigned> m_isClamped;				// map containg indices of clamped positions - defines boundary conditions
	std::vector<glm::vec4> m_restpos;

	glm::vec4 m_endPos;							//vec4 containing the Position of the last vertice of the rod
	
	std::vector<glm::vec3> m_edges;				// egdes = vertices - 1; segment between v[i + 1] and v[i]
	std::vector<glm::vec3> m_kb;				// curvature binormals at a vertex
	std::vector<glm::vec3> m_t;					// tangent vector for Bishop and Material frame
	std::vector<glm::vec3> m_u;					// u vector of the Bishop frame
	std::vector<glm::vec3> m_v;					// v vector of the Bishop frame
	std::vector<glm::vec3> m_m1;				// material axis m1(normal) of the Material frame
	std::vector<glm::vec3> m_m2;				// material axis m2(binormal) of the Material frame
	std::vector<glm::vec3> m_m1Rest;			// material axis m1(normal) of the Material frame while rod is in restposition
	std::vector<glm::vec3> m_m2Rest;			// material axis m2(binormal) of the Material frame while rod is in restposition

	std::vector<glm::vec3> m_force;				// force that is applied on a vertex of the centerline

	std::vector<float> m_restEdgeLength;		// length of edge at restposition
	std::vector<float> m_voronoiRegion;			// defines integrated length for voronoi region of integration Di = li/2
	std::vector<float> m_restRegionLength;		// li = |e^i-1| + |e^i|
	std::vector<glm::vec2> m_restOmegaprev;		// restOmegaprev[i] defines rest material curvature for kb[i] at e[i -1]
	std::vector<glm::vec2> m_restOmeganext;		// restOmeganext[i] defines rest material curvature for kb[i] at e[i]

	std::vector<glm::vec3> m_minusPsiTerm;		// Term for the gradient of psi with i = i - 1
	std::vector<glm::vec3> m_plusPsiTerm;		// Term for the gradient of psi with i = i + 1
	std::vector<glm::vec3> m_equalPsiTerm;		// Term for the gradient of psi with i = i
												
	std::vector<glm::mat3x3> m_minusGradKB;		// Term for the gradient of the curvature binormal for i = i - 1
	std::vector<glm::mat3x3> m_plusGradKB;		// Term for the gradient of the curvature binormal for i = i + 1
	std::vector<glm::mat3x3> m_equalGradKB;		// Term for the gradient of the curvature binormal for i = i

	glm::mat2 m_J = glm::mat2({ 0.0f, -1.0f }, { 1.0f, 0.0f }); //J acts on two dimensional vectors by counter-clockwise pi/2 rotation
	BoundaryCondition m_clampType;					//determines the boundary conditions for 

	float m_dEdThetaZero;

	std::vector<float> m_constraints;
	std::vector<float> m_constr_diag;
	std::vector<float> m_constr_off_diag;

	bool m_thetaOverTime = false;				// if set true in GUI the value of m_theta[last] increases per timestep
	bool m_externalForcesEnabled = true;
	bool m_gravityEnabled = true;						// gravity can be dis- or enabled in GUI
	bool m_windEnabled = false;
	bool m_dampingEnabled = false;

	//simple Collision stuff
	bool m_planeCollision = false;
	bool m_cubeCollision = false;
	bool m_sphereCollision = false;
	bool m_headCollision = false;
	float m_collisionCubeSize;
	float m_collisionSphereRadius;
	float m_collisionHeadRadius;
	glm::mat4 m_planeCollisionMat = glm::mat4(1.0f);
	glm::mat4 m_cubeCollisionMat = glm::mat4(1.0f);
	glm::mat4 m_sphereCollisionMat = glm::mat4(1.0f);
	glm::mat4 m_headCollisionMat = glm::mat4(1.0f);
	float m_rodThickness;

	float m_rodLength;							// contains length of the rod in rest shape

	glm::vec3 m_gravity = glm::vec3(0.0f, -9.82f, 0.0f);

	float m_timestep = 0.0001f;
};