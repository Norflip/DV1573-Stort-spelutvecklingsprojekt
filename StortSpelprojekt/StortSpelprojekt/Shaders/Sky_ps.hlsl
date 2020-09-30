#include "CommonBuffers.hlsl"
Texture2D diffuse : register(t0);	// for wic
TextureCube skymap : register(t2);	// for dds
SamplerState defaultSampler : register (s0);

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 lPosition : LPOSITION;
	float2 uv : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 diffuseMat = skymap.Sample(defaultSampler, input.lPosition);

	float4 diff = diffuse.Sample(defaultSampler, input.uv);

	return diff; //  float4(diffuseMat.r, diffuseMat.g * 2, diffuseMat.b, diffuseMat.a); //  float4(1, 0, 0, 1);
}