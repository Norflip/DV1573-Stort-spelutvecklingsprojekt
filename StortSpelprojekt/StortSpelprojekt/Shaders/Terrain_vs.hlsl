#include "CommonBuffers.hlsl"
#include "IO.hlsl"

Texture2D chunkData : register (t0);
Texture2D chunkNormalTexture : register (t1);

SamplerState m_samplerState : register(s0);

SamplerState NormalSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
	return float4(cross(va, vb).rgb, 0.0f);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;	

	// send in a constant buffer? 
	const float TERRAIN_SCALE = 10.0f;

	/*
		r channel: height [0-1]
		g channel: distance to road [0-1] 
	*/

	float4 position = input.position;
	float sampledHeight = chunkData.SampleLevel(m_samplerState, input.uv, 0).x;
	position.y += sampledHeight * TERRAIN_SCALE;

	output.worldPosition = mul(world, position).xyz;
	output.position = mul(mvp, position);
	
	float4 normal = GetNormal(input.uv, 1.0f / 65.0f, TERRAIN_SCALE);
	//float4 normal = float4(Unpack3DVector(tes.z), 0.0f);

	//float4 normal = chunkNormalTexture.SampleLevel(NormalSampler, input.uv, 0);

	output.normal = normalize(mul(world, input.normal).xyz);
	output.tangent = normalize(mul(world, input.tangent).xyz);
	return output;
}