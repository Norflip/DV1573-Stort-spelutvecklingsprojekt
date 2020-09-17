#include "CommonBuffers.hlsl"

cbuffer cb_Skeleton : register(b3)
{
	float4x4 bones[60];
};


struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	uint3 boneID    : BONEID;
	float3 skinWeight: SKINWEIGHT;
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

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;


	float4 p = mul(float4(input.position, 1), bones[input.boneIDs.x]) * skinWeight.x;
	p += mul(float4(input.position, 1), bones[input.boneIDs.y]) * skinWeight.y;
	p += mul(float4(input.position, 1), bones[input.boneIDs.z]) * skinWeight.z;

	float3 n = mul(input.normal, bones[input.boneIDs.x]).xyz * skinWeight.x;
	n += mul(input.normal, bones[input.boneIDs.y]).xyz * skinWeight.y;
	n += mul(input.normal, bones[input.boneIDs.z]).xyz * skinWeight.z;

	float3 t = mul(input.tangent, bones[input.boneIDs.x]).xyz * skinWeight.x;
	t += mul(input.tangent, bones[input.boneIDs.y]).xyz * skinWeight.y;
	t += mul(input.tangent, bones[input.boneIDs.z]).xyz * skinWeight.z;

	float3 b = mul(input.binormal, bones[input.boneIDs.x]).xyz * skinWeight.x;
	b += mul(input.binormal, bones[input.boneIDs.y]).xyz * skinWeight.y;
	b += mul(input.binormal, bones[input.boneIDs.z]).xyz * skinWeight.z;

	float4 outpos = mul(float4(p.xyz, 1), WVP);
	float3 outputWorldpos = mul(p,world[0]).xyz;











	output.uv = input.uv;
	output.worldPosition = outputWorldpos;
	output.position = outpos;
	

	output.normal = normalize(n);
	output.tangent = normalize(t);

	return output;
}