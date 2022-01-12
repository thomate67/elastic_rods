#version 450

layout (location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 test;

void main() 
{
    vec3 blub = test;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
}