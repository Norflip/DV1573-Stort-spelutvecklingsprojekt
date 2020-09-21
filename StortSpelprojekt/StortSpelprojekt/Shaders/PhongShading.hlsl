#include "CommonBuffers.hlsl"

float4 CalculateLight(PointLight pointLight, float3 normal, float3 objectPosition, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 light = pointLight.lightPosition - objectPosition;
	//float range = 25.0f; // Put this  in light object
	
	float distance = length(light);
	light = normalize(light);

	if (distance > pointLight.range)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float diffuseFactor = dot(light, normal);

	if (diffuseFactor > 0.0f)
	{
		float3 reflection = reflect(-light, normal);
		float shine = pow(max(dot(reflection, viewDirection), 0.0f), 10.0f); //matSpecular.w);


		diffuse = diffuseFactor * matDiffuse * pointLight.lightColor;
		specular = shine * matSpecular * pointLight.lightColor;
	}

	float attenuationFactor = 1.0f / pointLight.attenuation.x + (pointLight.attenuation.y * distance) + (pointLight.attenuation.z * (distance * distance));

	ambient = saturate(matAmbient * pointLight.lightColor * attenuationFactor);
	diffuse = saturate(diffuse * attenuationFactor);
	specular = saturate(specular * attenuationFactor);

	finalColor = ambient + diffuse + specular;

	return finalColor;
}