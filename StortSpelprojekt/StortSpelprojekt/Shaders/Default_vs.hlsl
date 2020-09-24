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
	output.worldPosition = mul(world, input.position).xyz;
	//output.worldPosition = mul(input.position, world).xyz;
	output.position = mul(mvp, input.position);
	//output.position = input.position * 0.5f;

//	output.position.x += input.id * 0.4f;
	//output.position.y += input.id * 0.4f;

	output.normal = normalize(mul(world, input.normal).xyz);
	output.tangent = normalize(mul(world, input.tangent).xyz);

	return output;
	
}