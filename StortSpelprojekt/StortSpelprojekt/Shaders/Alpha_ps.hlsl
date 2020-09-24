#include "PhongShading.hlsl"

Texture2D diffuseMap : register (t0);
Texture2D alphaMap : register (t1); //Maybe you can do this directly inside an alpha channel.

SamplerState defaultSampleType : register (s0);

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
	
	

	
	float4 textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

	

	float3 normalized = normalize(input.normal);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

	finalColor += CalculateDirectionalLight(sunDirection, normalized, viewDirection);

	for (int i = 0; i < nrOfPointLights; i++)
	{
		finalColor += CalculatePointLight(pointLights[i], normalized, input.worldPosition, viewDirection);
	}

	textureColor.a = alphaMap.Sample(defaultSampleType, input.uv).r;



	finalColor = saturate(finalColor * textureColor);

	//return float4(input.normal, 1.0f);
	return finalColor;
}