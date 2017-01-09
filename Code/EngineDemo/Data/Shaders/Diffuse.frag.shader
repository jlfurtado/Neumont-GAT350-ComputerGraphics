#version 430 core

in vec3 objectColor;
in vec3 fragmentNormal_WorldSpace;
in vec3 fragmentPosition_WorldSpace;
in vec3 fragmentLightPosition_WorldSpace;
in vec4 lightColor;
out vec4 fColor;

void main()
{
	fColor = lightColor;
}