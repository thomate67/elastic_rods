#version 450

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvAttribute;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 pass_position;
out vec3 pass_normal;

void main() 
{
    pass_position = viewMatrix * modelMatrix * position;
    pass_normal = vec3(transpose(inverse(viewMatrix * modelMatrix)) * vec4(normal, 0.0));

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
}
