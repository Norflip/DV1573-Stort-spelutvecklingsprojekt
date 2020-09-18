#include "CommonBuffers.hlsl"

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float3 normalized = normalize(input.normal);

	float range = 25.0f;
	float3 viewDirection = float3(0.0f, 0.0f, 0.0f) - input.worldPosition;

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 light = lightPosition - input.worldPosition;

	float distance = length(light);

	if (distance > range)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	light /= distance;

	ambient = matAmbient * lightColor;

	float diffuseFactor = dot(light, normalized);

	if (diffuseFactor > 0.0f)
	{
		float3 reflection = reflect(-light, normalized);
		//float shine = pow(max(dot(reflection, viewDirection), 0.0f), matSpecular.w);

		float shade = max(dot(reflection, viewDirection), 0.0f);
		float factor = pow(shade, 10.0f);

		diffuse = diffuseFactor * matDiffuse * lightColor;
		specular = factor * matSpecular * lightColor;
	}

	float attenuationFactor = 1.0f / attenuation.x + (attenuation.y * distance) + (attenuation.z * (distance * distance));

	diffuse = saturate(diffuse * attenuationFactor);
	specular = saturate(specular * attenuationFactor);
	ambient = saturate(ambient * attenuationFactor);

	finalColor = textureColor * (ambient + diffuse) + specular;

	/*float d = dot(input.normal, float3(1,1,0));
	d = (d + 1) / 2.0f;*/

	return finalColor;
	//return float4(input.normal, 1.0f);
	//return float4(d * input.uv.x, d * input.uv.y, 1.0f, 1.0f);
}
