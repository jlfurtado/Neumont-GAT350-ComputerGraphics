#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vertUV;

layout(location = 13) uniform mat4 transform;

out vec2 UV;

void main()
{
	gl_Position = transform * vec4(vPos, 1.0f);

	UV = vertUV;
}