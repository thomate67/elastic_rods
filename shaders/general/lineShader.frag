#version 450 core

uniform vec4 lineColor;

layout (location = 0) out vec4 color;

void main()
{
	color = lineColor;
}