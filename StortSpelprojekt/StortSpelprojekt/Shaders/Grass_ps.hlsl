#include "CommonBuffers.hlsl"
#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{
	float3 L = sunDirection;
	float3 T = input.grassDirection;

	float3 pos2Eye = normalize(cameraPosition - input.posToEye);

	float fresnel = pow((1 - saturate(dot(input.normal, pos2Eye))), 8);

	float3 N = input.normal;

	float2 uv = N.xy * 0.5f + 0.5f;
	uv.y = 1 - uv.y;

	float3 H = normalize(L + pos2Eye);
	float dotTH = dot(T,H);
	float sinTH = sqrt(1.0 - dotTH * dotTH);
	float dirAtten = smoothstep(-1,0,dotTH);

	float3 specular = 0.2 * dirAtten * pow(sinTH, 300) * float3(0.8,0.5,0);

	float3 colour = input.colour.rgb * lerp(0.5, 1.0, input.tex.y) + specular + fresnel * float3(0.9,0.9,0);

	return float4(colour,1);
}