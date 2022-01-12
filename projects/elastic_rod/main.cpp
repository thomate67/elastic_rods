#include <src/libraries/Utility/Definitions.h>
#include <src/libraries/Shader/ShaderProgram.h>
#include <src/libraries/Rendering/Camera.h>
#include<src/libraries/Utility/RenderLine.h>
#include<src/libraries/Utility/RenderBishopFrames.h>
#include<src/libraries/Utility/GenerateLinear.h>
#include<src/libraries/Utility/GenerateCurve.h>
#include<src/libraries/Utility/GenerateHelix.h>
#include<src/libraries/Geometry/RenderSphere.h>
#include<src/libraries/Geometry/RenderCone.h>
#include <src/libraries/Geometry/RenderCube.h>
#include <src/libraries/Geometry/RenderPlane.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <iostream>
#include <array>
#include <src/imGUI/imgui.h>
#include <src/imGUI/imgui_glfw.h>
#include <src/iconfont/IconsMaterialDesignIcons.h>
#include <src/libraries/Utility/tinyfiledialogs.h>
#include <src/nlohmann/json.hpp>
#include<filesystem>
#include <src/ElasticRods.h>
#include <src/RodRenderer.h>
#include <src/HairGenerator.h>
#include <src/RigidBody.h>
//#include <dependencies/include/lapacke/lapacke.h>
//#include <lapacke/lapacke.h>

std::ofstream log_file("log.txt", std::ios::app);
#define WIDTH 1280
#define HEIGHT 720

std::unique_ptr<ImGui::ImGui> gui;

/*****************************************Key Callbacks*****************************************/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		gui->keyCallback(window, key, scancode, action, mods);
		return;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
}

float sdSphere(glm::vec3 p)
{
	glm::vec3 pos = glm::vec3(0.0f, 2.0f, 0.0f);
	//glm::vec3 q = glm::inverse(pos + p);
	//return glm::length(p) - 1.0f;
	glm::vec3 b = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 q = glm::abs(p) - b;
	return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

using json = nlohmann::json;
namespace glm {
	void to_json(json& j, const glm::vec4& v) {
		//j = { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
		j = nlohmann::json::array();
		j[0] = v[0];
		j[1] = v[1];
		j[2] = v[2];
		j[3] = v[3];
	}

	void from_json(const json& j, glm::vec4& v) {
		//v.x = j.at("x").get<float>();
		//v.y = j.at("y").get<float>();
		//v.z = j.at("z").get<float>();
		//v.w = j.at("w").get<float>();
		v = glm::vec4(j[0], j[1], j[2], j[3]);
	}
}

int main()
{
	/*****************************************Init GLFW Stuff*****************************************/
	if (!glfwInit())
		exit(EXIT_FAILURE);
	GLFWwindow* window;
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);*/
	/*glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Elastic Rods", 0, 0);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glewInit();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 8.3f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, 0.0001f, 50.0f);

	/*****************************************Imgui Init***************************************************/
	gui = std::make_unique<ImGui::ImGui>(window, false);		//create gui object
	glfwSetKeyCallback(window, key_callback);
	ImGui::StyleColorsClassic();								//pick color style Classic/Dark/Light  or edit a completely own style have to look where this is done

	glfwSetMouseButtonCallback(
		window, [](GLFWwindow* w, const int button, const int action, const int m) {
			gui->mouseButtonCallback(w, button, action, m);
		});
	glfwSetScrollCallback(window,
		[](GLFWwindow* w, const double xoffset, const double yoffset) {
			gui->scrollCallback(w, xoffset, yoffset);
		});
	glfwSetCharCallback(window, [](GLFWwindow* w, const unsigned int c) {
		gui->charCallback(w, c);
		});

	//imgui parameter
	int shapeType = -1;
	int g_simulationState = -1;						// simulationState = 0 -> Simulation is running; simulationState = -1 -> Simulation is pausing
	float g_timestep = 0.0001f;						// Timestep for the simulation
	int g_drawAxis = -1;							//draw Axis for the rod x = 0; y = 1; z = 2
	float g_parabolaValueA = 0.2f;					// value a for the parabola
	int g_boundaryCondition = -1;
	bool g_fileLoaded = false;
	bool g_highRes = false;
	bool g_renderBishopFrame = false;
	bool g_renderMaterialFrame = false;
	float g_lastTheta = 0.0f;
	bool g_thetaOverTime = false;
	bool g_externalForcesEnabled = true;
	bool g_gravityOn = true;
	bool g_windOn = false;
	bool g_dampingOn = false;
	bool g_isGenerated = false;
	int g_maxRods = 200;
	bool g_createScenePlane = false;
	float g_scenePlaneYposition = -15.0f;
	bool g_createSceneCube = false;
	float g_sceneCubeSize = 1.0f;
	glm::vec3 g_sceneCubePos = glm::vec3(0.0f);
	bool g_createSceneSphere = false;
	float g_sceneSphereSize = 1.0f;
	int g_sceneSphereResolution = 20;
	glm::vec3 g_sceneSpherePos = glm::vec3(0.0f);
	bool g_rigidBody = false;
	int g_rbType = -1;
	float g_rbMass = 5.0f;
	bool g_renderVertecies = false;
	float g_vertexRadius = 0.035f;
	float g_rodRadius = 0.025f;
	int yViewport = 0;

	float g_lightPosX = 10.0f;
	float g_lightPosY = 10.0f;
	float g_lightPosZ = 10.0f;
	glm::vec3 lightPos = glm::vec3(g_lightPosX, g_lightPosY, g_lightPosZ);

	// colors
	glm::vec3 rodColor = glm::vec3(0.812f, 0.525f, 0.0f);
	glm::vec3 verticesColor = glm::vec3(0.0f, 0.357f, 0.812f);
	glm::vec3 scenePlaneColor = glm::vec3(0.522f, 0.522f, 0.522f);
	glm::vec3 sceneCubeColor = glm::vec3(0.467f, 0.498f, 0.522f);
	glm::vec3 sceneSphereColor = glm::vec3(0.522f, 0.518f, 0.467f);
	glm::vec3 headColor = glm::vec3(0.082f, 0.812f, 0.424f);
	glm::vec3 rigidBodyColor = glm::vec3(0.545f, 0.082f, 0.812f);


	glm::mat4 modelMatrix = glm::mat4(1.0f);

	std::unique_ptr<GenerateLinear> linearShape;
	std::unique_ptr<GenerateCurve> curvedShape;
	std::unique_ptr<GenerateHelix> helicalShape;

	BoundaryCondition boundaryType;
	ElasticRodParameter rodParameter;
	ElasticRod rod;
	std::unique_ptr<ElasticRod> elasticRod;

	int rodCount = 1;
	std::vector<std::unique_ptr<ElasticRod>> rods;

	std::vector<glm::vec4> vertices;
	std::vector<std::array<float, 4>> tmpVertices;
	std::vector<float> thetas;
	std::vector<glm::vec3> rodVel;
	std::vector<float> rodMass;

	int numVertices = 75;							//number of vertices for the Rod
	float rodLength = 9.29f;						//Length of the Rod
	float rodElementMass = 0.5f;
	float bendingModulus = 0.4f;
	float twistingModulus = 0.7f;
	int maxForce = 1000;

	//rigid-body coupling
	//std::shared_ptr<RenderCube> rbCube;
	RenderCube* rbCube;
	RenderSphere* rbSphere;
	std::unique_ptr<RigidBody> rbObject;

	//hair simulation stuff
	float g_headRadius = 1.0f;
	int g_headResolution = 20;
	float lengthVariance = 0.0f;
	int g_hairType = -1;
	bool g_headCollision = false;
	glm::vec3 g_headPos = glm::vec3(0.0f);

	std::unique_ptr<HairGenerator> strand;

	std::unique_ptr<RenderSphere> headSphere;
	std::vector<glm::vec4> headVertecies;
	std::vector<glm::vec3> headNormals;
	std::unique_ptr<RenderSphere> sceneSphere;
	std::unique_ptr<RenderPlane> scenePlane;
	std::unique_ptr<RenderCube> sceneCube;

	std::unique_ptr<RodRenderer> renderer = std::make_unique<RodRenderer>();
	renderer->setRenderMode(g_highRes);
	renderer->createBuffer();

	/*****************************************Load Shaders*****************************************/
	//shader for Bishop Frame and Material Frame
	ShaderProgram framesShader = ShaderProgram(SHADERS_PATH "/elastic_rods/framesShader.vert", SHADERS_PATH "/elastic_rods/framesShader.frag");
	framesShader.updateUniform("projectionMatrix", camera.projection());
	framesShader.updateUniform("modelMatrix", modelMatrix);

	//shader for elastic rod
	ShaderProgram edgesShader = ShaderProgram(SHADERS_PATH "/elastic_rods/rodEdges.vert", SHADERS_PATH "/elastic_rods/rodEdges.frag", SHADERS_PATH "/elastic_rods/rodEdges.geom");
	edgesShader.updateUniform("projectionMatrix", camera.projection());
	edgesShader.updateUniform("modelMatrix", modelMatrix);

	ShaderProgram verticesShader = ShaderProgram(SHADERS_PATH "/elastic_rods/rodVertices.vert", SHADERS_PATH "/elastic_rods/rodVertices.frag");
	verticesShader.updateUniform("projectionMatrix", camera.projection());
	verticesShader.updateUniform("modelMatrix", modelMatrix);

	//shader for scene objects
	ShaderProgram sceneSphereShader = ShaderProgram(SHADERS_PATH "/elastic_rods/sceneSphere.vert", SHADERS_PATH "/elastic_rods/sceneSphere.frag");
	sceneSphereShader.updateUniform("projectionMatrix", camera.projection());
	sceneSphereShader.updateUniform("modelMatrix", modelMatrix);

	ShaderProgram sceneCubeShader = ShaderProgram(SHADERS_PATH "/elastic_rods/sceneCube.vert", SHADERS_PATH "/elastic_rods/sceneCube.frag");
	sceneCubeShader.updateUniform("projectionMatrix", camera.projection());
	sceneCubeShader.updateUniform("modelMatrix", modelMatrix);

	ShaderProgram scenePlaneShader = ShaderProgram(SHADERS_PATH "/elastic_rods/scenePlane.vert", SHADERS_PATH "/elastic_rods/scenePlane.frag");
	scenePlaneShader.updateUniform("projectionMatrix", camera.projection());
	scenePlaneShader.updateUniform("modelMatrix", modelMatrix);

	//shader for rigid body objects
	ShaderProgram rigidBodyShader = ShaderProgram(SHADERS_PATH "/elastic_rods/rigidBody.vert", SHADERS_PATH "/elastic_rods/rigidBody.frag");
	rigidBodyShader.updateUniform("projectionMatrix", camera.projection());
	rigidBodyShader.updateUniform("modelMatrix", modelMatrix);

	//shader for head sphere
	ShaderProgram headSphereShader = ShaderProgram(SHADERS_PATH "/elastic_rods/headSphere.vert", SHADERS_PATH "/elastic_rods/headSphere.frag");
	headSphereShader.updateUniform("projectionMatrix", camera.projection());
	headSphereShader.updateUniform("modelMatrix", modelMatrix);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);


	/*****************************************Render Loop***************************************************/
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui->newFrame();
		{
			ImGui::Begin("Discrete Elastic Rods");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("");
			/***************************Save & Load Configuration***********************************/
			if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Save"))
			{
				const char* fileEnding = "*.json";
				const char* filePath = tinyfd_saveFileDialog("Save", nullptr, 1, &fileEnding, "JSON-Files");
				if (filePath)
				{
					nlohmann::json out;
					//rod config params
					out["shapeType"] = shapeType;
					out["drawAxis"] = g_drawAxis;
					out["verticesCount"] = numVertices;
					out["rodLength"] = rodLength;
					out["vertexMass"] = rodElementMass;
					out["bendingModulus"] = bendingModulus;
					out["twistingModulus"] = twistingModulus;
					out["maximumForce"] = maxForce;
					out["lastThetaAngle"] = g_lastTheta;
					out["thetaAngleOverTime"] = g_thetaOverTime;
					out["boundaryCondition"] = g_boundaryCondition;
					out["parabolaValueA"] = g_parabolaValueA;
					//simu
					out["timestep"] = g_timestep;
					out["externalEnabled"] = g_externalForcesEnabled;
					out["gravityEnabled"] = g_gravityOn;
					out["windEnabled"] = g_windOn;
					out["dampingEnabled"] = g_dampingOn;
					tmpVertices.resize(vertices.size());
					for (int i = 0; i < vertices.size(); i++)
					{
						tmpVertices[i][0] = vertices[i].x;
						tmpVertices[i][1] = vertices[i].y;
						tmpVertices[i][2] = vertices[i].z;
						tmpVertices[i][3] = vertices[i].w;
					}
					out["verticePositions"] = tmpVertices;

					{
						std::filesystem::path path = filePath;
						while (path.extension() != ".json")
						{
							path = path.string() + ".json";
						}
						std::ofstream output(path);
						output << std::setw(4) << out;
						std::cout << "Rod Configuration saved successfully.\n";
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_MDI_UPLOAD " Load"))
			{
				const char* fileEnding = "*.json";
				const char* filePath = tinyfd_openFileDialog("Load", nullptr, 1, &fileEnding, "JSON-Files", 0);

				if (filePath)
				{
					std::ifstream  ifs(std::filesystem::path(filePath).string());
					nlohmann::json json;
					try
					{
						ifs >> json;
						shapeType = json["shapeType"];
						std::vector<std::array<float, 4>> tmpVertLoad = json["verticePositions"];
						tmpVertices = tmpVertLoad;
						if (shapeType == 4)
						{
							rodLength = 0.0f;
							numVertices = tmpVertices.size();
							g_drawAxis = 0;
						}
						else
						{
							rodLength = json["rodLength"];
							numVertices = json["verticesCount"];
							g_drawAxis = json["drawAxis"];
						}
						rodElementMass = json["vertexMass"];
						bendingModulus = json["bendingModulus"];
						twistingModulus = json["twistingModulus"];
						maxForce = json["maximumForce"];
						g_lastTheta = json["lastThetaAngle"];
						g_thetaOverTime = json["thetaAngleOverTime"];
						g_boundaryCondition = json["boundaryCondition"];
						g_parabolaValueA = json["parabolaValueA"];
						g_timestep = json["timestep"];
						g_externalForcesEnabled = json["externalEnabled"];
						g_gravityOn = json["gravityEnabled"];
						g_windOn = json["windEnabled"];
						g_dampingOn = json["dampingEnabled"];
						
						g_fileLoaded = true;
						
					}
					catch (const nlohmann::json::exception&)
					{
						tinyfd_notifyPopup("Load Elastic Rod Configuration",
							"Selected file cannot be parsed as "
							"a valid json object.",
							"error");
					}
				}
			}

			/**********************************Rod Configuration****************************************/
			if (ImGui::CollapsingHeader("Create Scene Objects"))
			{
				if (ImGui::Checkbox("Plane", &g_createScenePlane));
				ImGui::SameLine();
				if (ImGui::Checkbox("Cube", &g_createSceneCube));
				ImGui::SameLine();
				if (ImGui::Checkbox("Sphere", &g_createSceneSphere));
				if (g_createScenePlane)
				{
					ImGui::DragFloat("Y position of the plane", &g_scenePlaneYposition, 0.1f, -30.0f, -0.1f);
					scenePlane = std::make_unique<RenderPlane>(glm::vec3(-20, 0.0, -20), glm::vec3(-20, 0.0, 20),
														   glm::vec3(20, 0.0, 20), glm::vec3(20, 0.0, -20));
				}

				if (g_createSceneCube)
				{
					ImGui::DragFloat("Cube size", &g_sceneCubeSize, 0.1f, 0.1f, 10.0f);
					ImGui::DragFloat3("Cube position", glm::value_ptr(g_sceneCubePos), 0.1f, -25.0f, 25.0f);
					sceneCube = std::make_unique<RenderCube>(g_sceneCubeSize, glm::vec3(0.0f, 0.0f, 0.0f));
				}
				if (g_createSceneSphere)
				{
					ImGui::DragFloat("Sphere radius", &g_sceneSphereSize, 0.1f, 0.1f, 10.0f);
					ImGui::DragInt("Sphere resolution", &g_sceneSphereResolution, 1, 8, 64);
					ImGui::DragFloat3("Sphere position", glm::value_ptr(g_sceneSpherePos), 0.1f, -25.0f, 25.0f);
					sceneSphere = std::make_unique<RenderSphere>(glm::vec4(0.0, 0.0, 0.0, 1.0), g_sceneSphereSize, g_sceneSphereResolution);
				}

				ImGui::Text("");
			}

			/**********************************Rod Configuration****************************************/
			ImGui::Text("Rod Configuration:");
			//const char* shapeTypes[] = { "Line_Shape", "Parabola_Shape", "Helical_Shape", "Freestyle" };
			//static int sType = reinterpret_cast<int>(shapeTypes);
			////static int sType = static_cast<int>(testType);
			//ImGui::PushItemWidth(160);
			//ImGui::Combo("Rod Type", &sType, shapeTypes, IM_ARRAYSIZE(shapeTypes));
			//shapeType = sType;


			ImGui::Text("Select shape of the rod");
			if (ImGui::RadioButton("Linear ", &shapeType, 0));
			ImGui::SameLine();
			if (ImGui::RadioButton("Curved ", &shapeType, 1));
			ImGui::SameLine();
			if (ImGui::RadioButton("Helical", &shapeType, 2));
			ImGui::SameLine();
			if (ImGui::RadioButton("Hair", &shapeType, 3));

			/*if (ImGui::CollapsingHeader("Rigid-Body Coupling"))
			{
				//ImGui::Checkbox("Rigid-Body Coupling", &g_rigidBody);
				if (ImGui::Checkbox("Rigid-Body Coupling", &g_rigidBody));
				if (g_rigidBody)
				{
					g_boundaryCondition = 3;
					
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Cube ", &g_rbType, 0));
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere", &g_rbType, 1));
				ImGui::DragFloat("Mass of Rigid-Body", &g_rbMass, 0.1f, 0.1f, 15.0f);
			}*/

			if (shapeType == 0 || shapeType == 1 || shapeType == 2)
			{
				ImGui::Checkbox("Rigid-Body Coupling", &g_rigidBody);
				ImGui::SameLine();
				if (ImGui::RadioButton("Cube ", &g_rbType, 0));
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere", &g_rbType, 1));
				if (g_rigidBody)
				{
					g_boundaryCondition = 3;
					ImGui::DragFloat("Mass of Rigid-Body", &g_rbMass, 0.1f, 0.1f, 15.0f);

				}

				ImGui::Text("Draw rod along the  -Axis");
				if (ImGui::RadioButton("X- ", &g_drawAxis, 0));
				ImGui::SameLine();
				if (ImGui::RadioButton("Y- ", &g_drawAxis, 1));
				ImGui::SameLine();
				if (ImGui::RadioButton("Z-", &g_drawAxis, 2));
			}

			if (shapeType == 3)
			{
				headSphere = std::make_unique<RenderSphere>(glm::vec4(0.0, 0.0, 0.0, 1.0), g_headRadius, g_headResolution);
				headVertecies = headSphere->getVertices();
				headNormals = headSphere->getNormals();
				ImGui::RadioButton("Straigh Hair", &g_hairType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Curly Hair", &g_hairType, 1);
				ImGui::DragInt("Number of Rods", &rodCount, 1, 1, g_maxRods);
				if (ImGui::DragFloat("Head-sphere radius", &g_headRadius, 0.1f, 0.0f, 5.0f));
				ImGui::DragInt("Head-sphere resolution", &g_headResolution, 1, 8, 64);
				ImGui::DragFloat3("Cube position", glm::value_ptr(g_headPos), 0.01f, -5.0f, 5.0f);
				g_headCollision = true;
				//g_boundaryCondition = 1;
			}

			if (shapeType == 1)
			{
				if (ImGui::DragFloat("a-value for parabola", &g_parabolaValueA, 0.01f, 0.0f, 2.0f));
			}

			ImGui::InputInt("Number of vertices", &numVertices, 1, 500);
			ImGui::DragFloat("Length of the rod", &rodLength, 0.01f, 0.8f, 15.0f);
			ImGui::DragFloat("Mass of Vertices", &rodElementMass, 0.01f, 0.01f, 15.0f);
			if (shapeType == 3)
			{
				if (ImGui::DragFloat("Hair length variance", &lengthVariance, 0.01f, 0.0f, 2.0f));
			}
			ImGui::DragFloat("Bending modulus", &bendingModulus, 0.01f, 0.0f, 250.0f, "%.6f");
			ImGui::DragFloat("Twisting modulus", &twistingModulus, 0.01f, 0.0f, 250.0f, "%.6f");
			ImGui::InputInt("Max. Force", &maxForce, 10, 1000);
			ImGui::Text("Change twist-angle for theta[n]:");
			ImGui::DragFloat("Fixed value", &g_lastTheta, 0.01f, 0.0f, 180.0f);
			ImGui::Checkbox("Over time", &g_thetaOverTime);
			ImGui::Text("Boundary Condition for rod");
			if (ImGui::RadioButton("Free ", &g_boundaryCondition, 0));
			ImGui::SameLine();
			if (ImGui::RadioButton("Oneside ", &g_boundaryCondition, 1));
			ImGui::SameLine();
			if (ImGui::RadioButton("Twoside", &g_boundaryCondition, 2));
			ImGui::SameLine();
			if (ImGui::RadioButton("Rigid-Body", &g_boundaryCondition, 3));

			if (ImGui::Button("Generate Elastic Rod") || g_fileLoaded == true)
			{
				rods.resize(rodCount);
				g_isGenerated = false;

				for (int i = 0; i < rodCount; i++)
				{
					if (shapeType == 0)
					{
						if (rodCount == 1)
						{
							linearShape = std::make_unique<GenerateLinear>(glm::vec3(-rodLength/2, 3.0f, 0.0f), rodLength, numVertices, g_drawAxis);
						}
						else
						{
							//linearShape = std::make_unique<GenerateLinear>(glm::sphericalRand(1.0f), rodLength, numVertices, g_drawAxis);
							linearShape = std::make_unique<GenerateLinear>(MathFunctions::quarterSphereRand(glm::vec3(0.0f, 2.0f, 0.0f), 1.0f), rodLength, numVertices, g_drawAxis);
						}

						vertices = linearShape->getVertices();

					}
					else if (shapeType == 1)
					{
						curvedShape = std::make_unique<GenerateCurve>(glm::vec3(0.0f, 2.0f, 0.0f), g_parabolaValueA, rodLength, numVertices, g_drawAxis);
						vertices = curvedShape->getVertices();
					}
					else if (shapeType == 2)
					{
						helicalShape = std::make_unique<GenerateHelix>(glm::vec3(-rodLength/2, 3.0f, 0.0f), rodLength, numVertices, g_drawAxis);
						vertices = helicalShape->getVertices();
					}
					else if (shapeType == 3)
					{
						strand = std::make_unique<HairGenerator>(MathFunctions::quarterSphereRand(g_headPos, 1.0f), rodLength, numVertices, g_hairType);
						vertices = strand->getVertices();
					}
					else if (shapeType == 4)
					{
						vertices.resize(tmpVertices.size());
						for (int j = 0; j < tmpVertices.size(); j++)
						{
							vertices[j].x = tmpVertices[j][0];
							vertices[j].y = tmpVertices[j][1];
							vertices[j].z = tmpVertices[j][2];
							vertices[j].w = tmpVertices[j][3];
						}
					}
					else
					{
						std::cout << "No shape for the elastic rod was selected!" << std::endl;
						break;
					}

					rodParameter = ElasticRodParameter(bendingModulus, twistingModulus, maxForce);

					if (g_boundaryCondition == 0)
					{
						boundaryType = BoundaryCondition::BC_FREE;
					}
					else if (g_boundaryCondition == 1)
					{
						boundaryType = BoundaryCondition::BC_FIXED_ONESIDE;
					}
					else if (g_boundaryCondition == 2)
					{
						boundaryType = BoundaryCondition::BC_FIXED_TWOSIDE;
					}
					else if (g_boundaryCondition == 3)
					{
						boundaryType = BoundaryCondition::BC_RIGIDBODY;
					}
					else
					{
						std::cout << "No boundary condition for the elastic rod was selected!" << std::endl;
						break;
					}

					thetas.resize(vertices.size() - 1);
					rodVel.resize(vertices.size());
					rodMass.resize(vertices.size());

					for (int i = 0; i < thetas.size(); i++)
					{

						thetas[i] = 0.0f;
					}

					for (int i = 0; i < vertices.size(); i++)
					{
						rodVel[i] = glm::vec3(0.0f);
						rodMass[i] = rodElementMass;
					}

					//rods[i] = ElasticRod(rodParameter, vertices, rodVel, rodMass, thetas, boundaryType);
					rods[i] = std::make_unique<ElasticRod>(rodParameter, vertices, rodVel, rodMass, thetas, boundaryType);

					if (g_lastTheta)
					{
						rods[i]->setLastTheta(0.0f);
						rods[i]->setLastTheta(g_lastTheta);
					}

					if (g_thetaOverTime)
					{
						rods[i]->setLastTheta(0.0f);
						rods[i]->setLastTheta(g_thetaOverTime);
					}

					if (g_rigidBody)
					{
						if (g_rbType == 0)
						{
							//rbCube = std::make_shared<RenderCube>(0.25f, glm::vec3(rods[i]->getEndPos()));
							rbCube = new RenderCube(0.25f, glm::vec3(0,0,0));
							rbObject = std::make_unique<RigidBody>(rbCube, glm::vec3(rods[i]->getEndPos()), g_rbMass);
						}
						else if (g_rbType == 1)
						{
							rbSphere = new RenderSphere(glm::vec4(0,0,0,1), 0.25f, 20);
							rbObject = std::make_unique<RigidBody>(rbSphere, glm::vec3(rods[i]->getEndPos()), g_rbMass);
						}

                           glm::vec3 t = rods[i]->getBishopT().back();
                           glm::mat3 m = glm::mat3(glm::lookAt(glm::vec3(0,0,0), t, glm::vec3(0,t.y > 0.5 ? 0 : 1,t.y > 0.5 ? 1 : 0)));
                           rbObject->setRotation(glm::quat_cast(glm::transpose(m)));
					}
				}
				g_maxRods = rodCount;
				g_fileLoaded = false;
				g_isGenerated = true;
			}

			/**************************************Simulation******************************************/
			if (ImGui::CollapsingHeader("Simulation"))
			{
				//ImGui::BeginVertical(1, ImVec2(0, 75));
				ImGui::Text("Simulate Discrete Elastic Rod");

				if (ImGui::RadioButton("Simulate", &g_simulationState, 0));
				if (g_simulationState == 0)
				{
					for (int i = 0; i < rodCount; i++)
					{
						if (g_rigidBody)
						{
							glm::vec3 torque_transfer = rods[i]->getM1Frame(numVertices - 2);
							torque_transfer = torque_transfer * rods[i]->getdEdTheta(numVertices - 2);
							//float blub = rods[i]->getdEdTheta(3);
							rbObject->applyForce(glm::vec3(0.0f, -9.81f, 0.0f) * g_rbMass);
							//rbObject->applyTorque(torque_transfer);
							rbObject->integrate(g_timestep);
							rods[i]->setExternalForceStatus(g_externalForcesEnabled, g_gravityOn, g_windOn, g_dampingOn);
							rods[i]->setPlaneCollision(g_createScenePlane, g_scenePlaneYposition, g_rodRadius);
							rods[i]->setCubeCollision(g_createSceneCube, g_sceneCubePos, g_sceneCubeSize, g_rodRadius);
							rods[i]->setSphereCollision(g_createSceneSphere, g_sceneSpherePos, g_sceneSphereSize, g_rodRadius);
							//for (int j = 0; j < 32; j++)
							//{
							//	rods[i]->simulate(g_timestep);
							//}
							rods[i]->simulate(g_timestep);
							rods[i]->move(window);

                            glm::vec3 position = 0.5f * (glm::vec3(rods[i]->getEndPos()) + glm::vec3(rbObject->getPosition()));
                            rbObject->setPosition(position);
                            rods[i]->setEndPos(glm::vec4(position, 1));

                            glm::vec3 velocity = 0.5f * (glm::vec3(rods[i]->getEndVel()) + rbObject->getLinearVelocity());
                            rbObject->setLinearVelocity(velocity);
                            rods[i]->setEndVel(velocity);

                            glm::mat3 m = glm::mat3_cast(rbObject->getRotation());
                            //glm::vec3 tangent = glm::normalize(m[2] + rods[i]->getBishopT().back());
                            //int l = rods[i]->m_pos.size();
                            //rods[i]->m_pos[l-1] = rods[i]->m_pos[l-2] - glm::vec4(glm::length(rods[i]->m_pos[l-1] - rods[i]->m_pos[l-2]) * tangent, 0);

                            glm::vec3 tangent = rods[i]->getBishopT().back();
                            m[2] = tangent;
                            rbObject->setRotation(glm::quat_cast(m));
                        }
						else
						{
							rods[i]->setExternalForceStatus(g_externalForcesEnabled, g_gravityOn, g_windOn, g_dampingOn);
							rods[i]->setPlaneCollision(g_createScenePlane, g_scenePlaneYposition, g_rodRadius);
							rods[i]->setCubeCollision(g_createSceneCube, g_sceneCubePos, g_sceneCubeSize, g_rodRadius);
							rods[i]->setSphereCollision(g_createSceneSphere, g_sceneSpherePos, g_sceneSphereSize, g_rodRadius);
							if (shapeType == 3)
							{
								rods[i]->setHeadCollision(g_headCollision, g_headPos, g_headRadius, g_rodRadius);
							}
							//for (int j = 0; j < 32; j++)
							//{
							//	rods[i]->simulate(g_timestep);
							//}
							rods[i]->simulate(g_timestep);
							rods[i]->move(window);
						}
					}

				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Pause", &g_simulationState, 1));
				//ImGui::EndVertical();

				if (ImGui::DragFloat("Timestep", &g_timestep, 0.00001f, 0.00001f, 0.0025f, "%.5f"));

				ImGui::Checkbox("External Forces", &g_externalForcesEnabled);
				if (g_externalForcesEnabled)
				{
					ImGui::Checkbox("Gravity", &g_gravityOn);
					ImGui::SameLine();
					ImGui::Checkbox("Wind", &g_windOn);
					ImGui::SameLine();
					ImGui::Checkbox("Damping", &g_dampingOn);
				}
			}

			/*********************************Rendering Options***************************************/
			if (ImGui::CollapsingHeader("Rendering"))
			{
				//ImGui::BeginVertical(1, ImVec2(0, 75));
				ImGui::Text("rendering options for Elastic Rod");

				ImGui::Checkbox("Render Bishop Frames", &g_renderBishopFrame);
				ImGui::SameLine();
				ImGui::Checkbox("Render Material Frames", &g_renderMaterialFrame);
				ImGui::Text("");
				ImGui::Checkbox("High Resolution", &g_highRes);
				ImGui::Checkbox("Render vertecies", &g_renderVertecies);
			}

			/*********************************Visual Options***************************************/

			if (ImGui::CollapsingHeader("Visual"))
			{
				ImGui::Text("Elastic Rod");
				if (ImGui::ColorEdit3("Rod Color", glm::value_ptr(rodColor)))
				{
					edgesShader.updateUniform("diffColor", rodColor);
				}
				ImGui::DragFloat("Rod Radius", &g_rodRadius, 0.001f, 0.005f, 0.1f);

				if (ImGui::ColorEdit3("Vertex Color", glm::value_ptr(verticesColor)))
				{
					verticesShader.updateUniform("diffColor", verticesColor);
				}
				ImGui::DragFloat("Vertex Radius", &g_vertexRadius, 0.001f, 0.006f, 0.12f);
				ImGui::Text("");

				ImGui::Text("Scene Objects");
				if (ImGui::ColorEdit3("Plane Color", glm::value_ptr(scenePlaneColor)))
				{
					scenePlaneShader.updateUniform("diffColor", scenePlaneColor);
				}

				if (ImGui::ColorEdit3("Cube Color", glm::value_ptr(sceneCubeColor)))
				{
					sceneCubeShader.updateUniform("diffColor", sceneCubeColor);
				}

				if (ImGui::ColorEdit3("Sphere Color", glm::value_ptr(sceneSphereColor)))
				{
					sceneSphereShader.updateUniform("diffColor", sceneSphereColor);
				}

				ImGui::Text("Rigid Body Objects");
				if (ImGui::ColorEdit3("Rigid Body Color", glm::value_ptr(rigidBodyColor)))
				{
					rigidBodyShader.updateUniform("diffColor", rigidBodyColor);
				}

				ImGui::Text("Head Object");
				if (ImGui::ColorEdit3("Head Color", glm::value_ptr(headColor)))
				{
					headSphereShader.updateUniform("diffColor", headColor);
				}

				ImGui::Text("LightPosition");
				if (ImGui::DragFloat("Light position x", &g_lightPosX, 0.1f, -50.0f, 50.0f))
				{
					lightPos = glm::vec3(g_lightPosX, g_lightPosY, g_lightPosZ);
					headSphereShader.updateUniform("lightPosition", lightPos);
					rigidBodyShader.updateUniform("lightPosition", lightPos);
					scenePlaneShader.updateUniform("lightPosition", lightPos);
					sceneCubeShader.updateUniform("lightPosition", lightPos);
					sceneSphereShader.updateUniform("lightPosition", lightPos);
				}
				if (ImGui::DragFloat("Light position y", &g_lightPosY, 0.1f, -50.0f, 50.0f))
				{
					lightPos = glm::vec3(g_lightPosX, g_lightPosY, g_lightPosZ);
					headSphereShader.updateUniform("lightPosition", lightPos);
					rigidBodyShader.updateUniform("lightPosition", lightPos);
					scenePlaneShader.updateUniform("lightPosition", lightPos);
					sceneCubeShader.updateUniform("lightPosition", lightPos);
					sceneSphereShader.updateUniform("lightPosition", lightPos);
				}
				if (ImGui::DragFloat("Light position z", &g_lightPosZ, 0.1f, -50.0f, 50.0f))
				{
					lightPos = glm::vec3(g_lightPosX, g_lightPosY, g_lightPosZ);
					headSphereShader.updateUniform("lightPosition", lightPos);
					rigidBodyShader.updateUniform("lightPosition", lightPos);
					scenePlaneShader.updateUniform("lightPosition", lightPos);
					sceneCubeShader.updateUniform("lightPosition", lightPos);
					sceneSphereShader.updateUniform("lightPosition", lightPos);
				}
				
			}

		}

		ImGui::Text("");
		ImGui::End();

		camera.update(window);
		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);
		int yViewport = m_viewport[2];

		if (g_isGenerated)
		{
			if (shapeType == 3)
			{
				glm::mat4 headModelMatrix = glm::translate(glm::mat4(1.0f), g_headPos);
				headSphereShader.use();
				headSphereShader.updateUniform("lightPosition", lightPos);
				headSphereShader.updateUniform("viewMatrix", camera.view());
				headSphereShader.updateUniform("modelMatrix", headModelMatrix);
				headSphereShader.updateUniform("diffColor", headColor);
				headSphere->render();
			}

			for (int i = 0; i < rodCount; i++)
			{
				if (g_renderBishopFrame)
				{
					RenderBishopFrames frames = RenderBishopFrames(rods[i]->getVertices(), rods[i]->getEdges(), rods[i]->getBishopT(), rods[i]->getBishopU(), rods[i]->getBishopV());
					framesShader.use();
					framesShader.updateUniform("viewMatrix", camera.view());
					framesShader.updateUniform("lineColor", tangentColor);
					frames.renderTangents();
					framesShader.updateUniform("lineColor", normalColor);
					frames.renderNormals();
					framesShader.updateUniform("lineColor", binormalColor);
					frames.renderBinormals();
				}

				if (g_renderMaterialFrame)
				{
					RenderBishopFrames material = RenderBishopFrames(rods[i]->getVertices(), rods[i]->getEdges(), rods[i]->getBishopT(), rods[i]->getMaterial1(), rods[i]->getMaterial2());
					framesShader.use();
					//render material frame axis m1 and m2
					framesShader.updateUniform("viewMatrix", camera.view());
					framesShader.updateUniform("lineColor", tangentColor);
					material.renderTangents();
					framesShader.updateUniform("lineColor", m1Color);
					material.renderNormals();
					framesShader.updateUniform("lineColor", m2Color);
					material.renderBinormals();
					glBindVertexArray(0);
				}

				if (g_highRes)
				{
					//hier kommt die richtige darstellung rein, oder aber ich switche und mache High Resolution Default und Low Resolution eine option
				}


				edgesShader.use();
				edgesShader.updateUniform("viewMatrix", camera.view());
				edgesShader.updateUniform("diffColor", rodColor);
				edgesShader.updateUniform("lineWidth", g_rodRadius);
				edgesShader.updateUniform("lightPosition", lightPos);
				//glPointSize(5.0f);
				renderer->setVertices(rods[i]->getVertices());
				renderer->render();

				verticesShader.use();
				verticesShader.updateUniform("viewMatrix", camera.view());
				verticesShader.updateUniform("diffColor", verticesColor);
				verticesShader.updateUniform("viewportY", yViewport);
				verticesShader.updateUniform("vertexRadius", g_vertexRadius);
				verticesShader.updateUniform("lightPosition", lightPos);

				if (g_renderVertecies)
				{
					
					renderer->renderVertices();
				}

				//vbo_vertices = rods[i]->getVertices();
				//glBindBuffer(GL_ARRAY_BUFFER, test_vbo);
				//glBufferData(GL_ARRAY_BUFFER, vbo_vertices.size() * sizeof(glm::vec4), &vbo_vertices[0], GL_STATIC_DRAW);
				//
				//glLineWidth(5.5f);
				//glBindVertexArray(test_vao);
				//glDrawArrays(GL_LINE_STRIP, 0, vbo_vertices.size());

				if (g_rigidBody)
				{
                       glm::mat4 rbmodelMatrix = glm::translate(rbObject->getPosition()) * glm::mat4_cast(rbObject->getRotation());
                       if (g_rbType == 0)
					{
						rigidBodyShader.use();
						rigidBodyShader.updateUniform("lightPosition", lightPos);
						rigidBodyShader.updateUniform("viewMatrix", camera.view());
						//rbmodelMatrix = glm::mat4_cast(rbObject->getRotation());
						//std::cout << "macht die quaternion etwas: " << glm::length(rbObject->getRotation()) << std::endl;
						rigidBodyShader.updateUniform("modelMatrix", rbmodelMatrix);
						rigidBodyShader.updateUniform("diffColor", rigidBodyColor);
						rbCube->render();
					}
					else if (g_rbType == 1)
					{
						rigidBodyShader.use();
						rigidBodyShader.updateUniform("lightPosition", lightPos);
						rigidBodyShader.updateUniform("viewMatrix", camera.view());
						rigidBodyShader.updateUniform("modelMatrix", rbmodelMatrix);
						rigidBodyShader.updateUniform("diffColor", rigidBodyColor);
						rbSphere->render();
					}
				}
			}
		}

		if (g_createScenePlane)
		{
			glm::mat4 planeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, g_scenePlaneYposition, 0.0f));
			scenePlaneShader.use();
			scenePlaneShader.updateUniform("lightPosition", lightPos);
			scenePlaneShader.updateUniform("viewMatrix", camera.view());
			scenePlaneShader.updateUniform("modelMatrix", planeModelMatrix);
			scenePlaneShader.updateUniform("diffColor", scenePlaneColor);
			scenePlane->render();
		}

		if (g_createSceneCube)
		{
			glm::mat4 cubeModelMatrix = glm::translate(glm::mat4(1.0f), g_sceneCubePos);
			sceneCubeShader.use();
			sceneCubeShader.updateUniform("lightPosition", lightPos);
			sceneCubeShader.updateUniform("viewMatrix", camera.view());
			sceneCubeShader.updateUniform("modelMatrix", cubeModelMatrix);
			sceneCubeShader.updateUniform("diffColor", sceneCubeColor);
			sceneCube->render();
		}
		if (g_createSceneSphere)
		{
			glm::mat4 sphereModelMatrix = glm::translate(glm::mat4(1.0f), g_sceneSpherePos);
			sceneSphereShader.use();
			sceneSphereShader.updateUniform("lightPosition", lightPos);
			sceneSphereShader.updateUniform("viewMatrix", camera.view());
			sceneSphereShader.updateUniform("modelMatrix", sphereModelMatrix);
			sceneSphereShader.updateUniform("diffColor", sceneSphereColor);
			sceneSphere->render();
		}

		//render GUI
		gui->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}