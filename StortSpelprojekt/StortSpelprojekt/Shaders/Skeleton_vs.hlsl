#include "CommonBuffers.hlsl"
#include "IO.hlsl"

VS_OUTPUT main(VS_INPUT_SKELETON input)
{
	VS_OUTPUT output;

	float w1 = saturate(1 - (input.skinWeight.y + input.skinWeight.z));
	float w2 = saturate(1 - (w1 + input.skinWeight.z));
	float w3 = saturate(1 - (w2 + w1));

	float4 p = mul(float4(input.position,1), bones[input.boneID.x]) * w1;
	p += mul(float4(input.position,1), bones[input.boneID.y]) * w2;
	p += mul(float4(input.position,1), bones[input.boneID.z]) * w3;

	float3 n = mul(float4(input.normal,0), bones[input.boneID.x]).xyz * w1;
	n += mul(float4(input.normal, 0), bones[input.boneID.y]).xyz * w2;
	n += mul(float4(input.normal, 0), bones[input.boneID.z]).xyz * w3;

	float3 t = mul(float4(input.tangent, 0), bones[input.boneID.x]).xyz * w1;
	t += mul(float4(input.tangent, 0), bones[input.boneID.y]).xyz * w2;
	t += mul(float4(input.tangent, 0), bones[input.boneID.z]).xyz * w3;

	float3 b = mul(float4(input.binormal, 0), bones[input.boneID.x]).xyz * w1;
	b += mul(float4(input.binormal, 0), bones[input.boneID.y]).xyz * w2;
	b += mul(float4(input.binormal, 0), bones[input.boneID.z]).xyz * w3;

	float4 outpos = mul(float4(p.xyz, 1), transpose(mvp));
	float3 outputWorldpos = mul(p, transpose(world)).xyz;


	output.uv = input.uv;
	output.worldPosition = outputWorldpos;
	output.position = outpos;
	

	output.normal = normalize(n);
	output.tangent = normalize(t);

	return output;
}