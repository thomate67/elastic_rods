#version 450 core

uniform vec3 diffColor;
uniform vec3 lightPosition;

in vec4 passWorldPos;
in vec4 passPos;
in vec4 tangent;
in vec2 passUV;

layout (location = 0) out vec4 color;
float pi = 3.1415926535897;
void main()  
{
	vec3 lightDir = lightPosition;
	vec3 lightColor = vec3(1.0f,1.0f,1.0f);
	vec3 baseColor = vec3(0.1f,0.1f,0.1f);
	vec3 specColor = vec3(1.0f,1.0f,1.0f);
	
	float x = (cos(passUV.y * pi) + pi) * 0.2;
	float y = (sin(passUV.y * pi) + pi) * 0.2;
	vec3 passNormal = vec3(x, y, 0.0f);
	
	// Diffuse term
	lightDir = normalize(lightDir);
	float cos_phi = max(dot(passNormal, lightDir), 0.0f);

	// Specular term
	vec3 eye = normalize(-vec3(passPos));
	vec3 reflection = normalize(reflect(-lightDir, passNormal));
	float cos_psi_n = pow(max(dot(reflection, eye), 0.0f), 15);
	float diffuseShade = max(0.0f, dot(passNormal, normalize(lightDir)));

	//sum up colors
	color.rgb = baseColor;
	color.rgb += diffColor * vec3(diffuseShade, diffuseShade, diffuseShade) * lightColor;
	color.a = 1.0f;
}