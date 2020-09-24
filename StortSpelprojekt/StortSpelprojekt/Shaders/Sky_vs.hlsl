
#include "CommonBuffers.hlsl"

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	uint3 boneID    : BONEID;
	float3 skinWeight : SKINWEIGHT;
	uint id	: SV_InstanceID;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 lPosition : LPOSITION;
	float2 uv : TEXCOORD0;
};

PixelInputType main (VertexInputType input)
{
	PixelInputType output;

	output.position = mul(mvp, input.position);
	output.position = output.position.xyww;		 // look at this later

	output.lPosition = mul(world, input.position);

	output.uv = input.uv;

	return output;
}