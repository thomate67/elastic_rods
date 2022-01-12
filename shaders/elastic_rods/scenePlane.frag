#version 450 core

uniform vec3 diffColor;

in vec4 pass_position;
in vec3 pass_normal;

uniform vec3 lightPosition;

layout (location = 0) out vec4 color;

void main(void)
{
	vec3 baseColor = vec3(0.1f,0.1f,0.1f);
	
    //compute the light vector as the normalized vector between 
    //the vertex position and the light position:
    vec3 lightVector = normalize(lightPosition - pass_position.xyz);

    //compute the eye vector as the normalized negative vertex position in camera coordinates:
    vec3 eye = normalize(-pass_position.xyz);
    
    //compute the normalized reflection vector using GLSL's built-in reflect() function:
    vec3 reflection = normalize(reflect(-lightVector, normalize(pass_normal)));

    //varables used in the phong lighting model:
    float phi = max(dot(pass_normal, lightVector), 0);
    float psi = pow(max(dot(reflection, eye), 0), 15);

    vec3 ambientColor = vec3(0.5f, 0.0f, 0.15f);
    vec3 diffuseColor = vec3(0.5f, 0.0f, 0.15f);
    vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);

	color = vec4(baseColor + phi * diffColor + psi * specularColor, 1);
}
