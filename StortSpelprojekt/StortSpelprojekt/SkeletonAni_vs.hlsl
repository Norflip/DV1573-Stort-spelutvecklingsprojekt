#include "CommonBuffers.hlsl"

struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 boneID	: BONEID; //This should really be uint3 but the inputlayout complains.
	float3 skinWeight : SKINWEIGHT;
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

	float w1 = saturate(1 - (input.skinWeight.y + input.skinWeight.z)); //This is unesseracy, it fixes potential padding issues.
	float w2 = saturate(1 - (w1 + input.skinWeight.z));
	float w3 = saturate(1 - (w2 + w1));



	float4 p = mul(float4(input.position.xyz, 1), bones[input.boneID.x]) * w1; //Even if I have 2 or 1 it's plus 0
	p += mul(float4(input.position.xyz, 1), bones[input.boneID.y]) * w2;
	p += mul(float4(input.position.xyz, 1), bones[input.boneID.z]) * w3;

	float3 n = mul(input.normal, (float3x3)bones[input.boneID.x]) * w1;
	n += mul(input.normal, (float3x3)bones[input.boneID.y]) * w2;
	n += mul(input.normal, (float3x3)bones[input.boneID.z]) * w3;

	float3 t = mul(input.tangent, (float3x3)bones[input.boneID.x]) * w1;
	t += mul(input.tangent, (float3x3)bones[input.boneID.y]) * w2;
	t += mul(input.tangent, (float3x3)bones[input.boneID.z]) * w3;

	
	float4 outPosition = mul(float4(p.xyz, 1), mvp);
	float3 outWorldPosition = mul(p.xyz, (float3x3)world);



	output.position = outPosition;
	output.worldPosition = outWorldPosition;
	output.uv = input.uv;


	output.normal = normalize(mul(n, (float3x3)world));
	output.tangent = normalize(mul(t, (float3x3)world));
	

	return output;
}