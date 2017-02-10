#version 430 core

in vec3 lightColor;
in vec3 fragNormWorld;
in vec3 fragPosWorld;
out vec4 fColor;

struct FogInfo
{
	float minDist;
	float maxDist;
	vec3 color;
};

layout(location = 18) uniform FogInfo fog;
layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 5) uniform vec3 ambientLightIntensity;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 8) uniform vec3 specularLightIntensity;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(lightPos_WorldSpace - fragPosWorld);
	normalNormEye = normalize(fragNormWorld);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	float distance = length(fragPosWorld - cameraPosition_WorldSpace);
	float fogFactor = clamp((fog.maxDist - distance) / (fog.maxDist - fog.minDist), 0.0f, 1.0f);

	vec3 totalLight = mix(fog.color, ambientLight + diffuseLight + specularLight, fogFactor);
	return clamp((totalLight)* lightColor, 0.0f, 1.0f);
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);
	vec3 light = CalculatePhongLight(positionEye, normalNormEye);
	fColor = vec4(light, 1.0f);
}