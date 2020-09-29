#include "CommonBuffers.hlsl"

Texture2D chunkData : register (t0);
Texture2D chunkNormalTexture : register (t1);

SamplerState m_samplerState : register(s0);

SamplerState NormalSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	uint id	: SV_InstanceID;
};
 
struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

float4 GetNormal(float2 uv, float texel, float height)
{
	//texel is one uint size, ie 1.0/texture size
	float t0 = chunkData.SampleLevel(m_samplerState, uv + float2(-texel, 0), 0).x * height;
	float t1 = chunkData.SampleLevel(m_samplerState, uv + float2(texel, 0), 0).x * height;
	float t2 = chunkData.SampleLevel(m_samplerState, uv + float2(0, -texel), 0).x * height;
	float t3 = chunkData.SampleLevel(m_samplerState, uv + float2(0, texel), 0).x * height;

	float3 va = normalize(float3(1.0f, 0.0f, t1 - t0));
	float3 vb = normalize(float3(0.0f, 1.0f, t3 - t2));
	return float4(cross(va, vb).rbg, 0.0f);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;	

	// send in a constant buffer? 
	const float terrainHeight = 10.0f;

	/*
		r channel: height [0-1]
		g channel: distance to road [0-1] 
	*/

	float4 tes = chunkData.SampleLevel(m_samplerState, input.uv, 0);
	input.position.y += tes.x * terrainHeight;

	output.worldPosition = mul(world, input.position).xyz;
	output.position = mul(mvp, input.position);
	
	float4 normal = GetNormal(input.uv, 1.0f / 65.0f, terrainHeight);
	//float4 normal = float4(Unpack3DVector(tes.z), 0.0f);

	//float4 normal = chunkNormalTexture.SampleLevel(NormalSampler, input.uv, 0);

	output.normal = normalize(mul(normal, world).xyz);
	output.tangent = normalize(mul(input.tangent, world).xyz);
	return output;
}