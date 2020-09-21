#include "Light.hlsl"

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

	float4 finalColor = CalculateLight(normalized, input.worldPosition, viewDirection);


	return finalColor;
}
