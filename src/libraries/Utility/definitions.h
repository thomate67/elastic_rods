#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>
#include <set>
#include <memory>
#include <random>
#include <map>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>
#include <src/libraries/Math/MathFunctions.h>

const static float ERR = 1e-6f;
const static float THRESHOLD = 0.00001f;

#define M_PI 3.14159265358979323846264338327950288f

const static float pi_over_2 = M_PI / 2;

#define tangentColor glm::vec4(0.742f, 0.0f, 0.0f, 1.0f)
#define normalColor glm::vec4(0.0f, 0.5078f, 0.0f, 1.0f)
#define binormalColor glm::vec4(0.0f, 0.0f, 0.879f, 1.0f)
#define m1Color glm::vec4(0.95f, 0.5f, 0.0f, 1.0f)
#define m2Color glm::vec4(0.0f, 0.4f, 0.797f, 1.0f)

#endif