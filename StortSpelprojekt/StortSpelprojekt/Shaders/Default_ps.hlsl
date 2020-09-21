#include "PhongShading.hlsl"

Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);

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
	float4 normalColor = normalMap.Sample(defaultSampleType, input.uv);
	
	float3 normalized = normalize(input.normal);

	float3 viewDirection = float3(0.0f, 0.0f, 0.0f) - input.worldPosition;

	float4 finalColor = CalculateLight(pointLights[0], normalized, input.worldPosition, viewDirection);

	finalColor *= textureColor;

	return finalColor;

}
