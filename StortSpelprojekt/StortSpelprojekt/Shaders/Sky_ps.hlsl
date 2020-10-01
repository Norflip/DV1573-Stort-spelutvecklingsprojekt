#include "CommonBuffers.hlsl"

Texture2D day : register(t2);		// for wic
Texture2D sunset : register(t3);	// for wic
Texture2D night : register(t4);		// for wic
Texture2D end : register(t5);		// for wic

SamplerState defaultSampler : register (s0);

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 lPosition : LPOSITION;
	float2 uv : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 diff;
	float4 diff2;
	float4 final;

	// different id, different lerps between textures
	if (id == 0)
	{
		diff = day.Sample(defaultSampler, input.uv);
		diff2 = sunset.Sample(defaultSampler, input.uv);
		final = lerp(diff, diff2, factor);
	}
	if ( id == 1)
	{
		diff = sunset.Sample(defaultSampler, input.uv);
		diff2 = night.Sample(defaultSampler, input.uv);
		final = lerp(diff, diff2, factor);
	}
	if (id == 2)
	{
		diff = night.Sample(defaultSampler, input.uv);
		diff2 = end.Sample(defaultSampler, input.uv);
		final = lerp(diff, diff2, factor);
	}
	if (id == 3)
	{
		diff = end.Sample(defaultSampler, input.uv);
		diff2 = float4(diff.r, diff.g, diff.b, diff.a);
		final = lerp(diff, diff2, factor);
	}

	return final; //  float4(diffuseMat.r, diffuseMat.g * 2, diffuseMat.b, diffuseMat.a); //  float4(1, 0, 0, 1);
}