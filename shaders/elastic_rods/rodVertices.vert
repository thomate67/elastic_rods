#version 450

layout (location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform int viewportY;
uniform float vertexRadius;

out vec3 passPosition;
out float passPointSize;
out mat4 passProjMat;

void main() 
{
	vec4 pos = viewMatrix * modelMatrix * position;
	float pointScale = viewportY * 0.7f * projectionMatrix[1][1];
	passPosition = pos.xyz;
	passProjMat = projectionMatrix;
    gl_Position = projectionMatrix * pos;
	//gl_PointSize = vertexRadius;
	gl_PointSize = (vertexRadius * pointScale) / gl_Position.w;
	passPointSize = vertexRadius;
}