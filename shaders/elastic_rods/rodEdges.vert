#version 450

layout (location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float lineWidth;

out vec4 passPos_G;
flat out float passRadius_G;

void main() 
{
	passRadius_G = lineWidth;
	passPos_G = modelMatrix * position;
    gl_Position = projectionMatrix * viewMatrix * passPos_G;
	gl_PointSize = lineWidth;
}