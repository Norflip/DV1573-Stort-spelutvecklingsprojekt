#include "CommonBuffers.hlsl"

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
	float4x4 instanceWorld : INSTANCEWORLD;
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
	output.uv = input.uv;
	output.worldPosition = mul(input.instanceWorld, input.position).xyz;
	
	output.position = mul(vp, float4(output.worldPosition,1));
	

	output.normal = normalize(mul(world, float4(input.normal,0)).xyz);
	output.tangent = normalize(mul(world, float4(input.tangent,0)).xyz);

	return output;
}