#include "CommonBuffers.hlsl"
//#include "PSBlendModes.hlsl"
Texture2D diffuse : register(t0);
TextureCube skymap : register(t2);
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



	//EMIL TEST
	//float3 blue = float3(0, 0, 1);
	//float3 testColor = PSColorMode(diff.rgb, GetH(blue), GetS(blue), GetL(blue), 0.5);


	//return float4(testColor, 1);
}