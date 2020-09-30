#include "CommonBuffers.hlsl"
#include "IO.hlsl"

cbuffer cb_Skeleton : register(b3)
{
	float4x4 bones[60];
};

VS_OUTPUT main(VS_INPUT_SKELETON input)
{
	VS_OUTPUT output;


	float4 p = mul(input.position, bones[input.boneID.x]) * input.skinWeight.x;
	p += mul(input.position, bones[input.boneID.y]) * input.skinWeight.y;
	p += mul(input.position, bones[input.boneID.z]) * input.skinWeight.z;

	float3 n = mul(float4(input.normal,0), bones[input.boneID.x]).xyz * input.skinWeight.x;
	n += mul(float4(input.normal, 0), bones[input.boneID.y]).xyz * input.skinWeight.y;
	n += mul(float4(input.normal, 0), bones[input.boneID.z]).xyz * input.skinWeight.z;

	float3 t = mul(float4(input.tangent, 0), bones[input.boneID.x]).xyz * input.skinWeight.x;
	t += mul(float4(input.tangent, 0), bones[input.boneID.y]).xyz * input.skinWeight.y;
	t += mul(float4(input.tangent, 0), bones[input.boneID.z]).xyz * input.skinWeight.z;

	float3 b = mul(float4(input.binormal, 0), bones[input.boneID.x]).xyz * input.skinWeight.x;
	b += mul(float4(input.binormal, 0), bones[input.boneID.y]).xyz * input.skinWeight.y;
	b += mul(float4(input.binormal, 0), bones[input.boneID.z]).xyz * input.skinWeight.z;

	float4 outpos = mul(float4(p.xyz, 1), mvp);
	float3 outputWorldpos = mul(p,world).xyz;











	output.uv = input.uv;
	output.worldPosition = outputWorldpos;
	output.position = outpos;
	

	output.normal = normalize(n);
	output.tangent = normalize(t);

	return output;
}