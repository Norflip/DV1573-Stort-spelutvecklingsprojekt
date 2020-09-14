#include "CommonBuffers.hlsl"

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

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;	
	output.worldPosition = mul(input.position, world).xyz;
	output.position = mul(input.position, mvp);

//	output.position.x += input.id * 0.4f;
	//output.position.y += input.id * 0.4f;

	output.normal = normalize(mul(input.normal, world).xyz);
	output.tangent = normalize(mul(input.tangent, world).xyz);

	return output;
}