#include "CommonBuffers.hlsl"

cbuffer NoiseBuffer
{
	float frameTime;
	float3 scrollingSpeed;
	float3 scales;
	float padding;
};

struct VertexInput
{
	float4 position : POSITION;
	float3 tex : TEXCOORD0;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output = (VertexOutput)0;

	input.position.w = 1.0f;
	output.position = mul(mvp, input.position);

	output.tex = input.tex;

	output.texCoord1 = (input.tex * scales.x);
	output.texCoord1.y = output.texCoord1.y + (frameTime * scrollingSpeed.x);

	output.texCoord2 = (input.tex * scales.y);
	output.texCoord2.y = output.texCoord2.y + (frameTime * scrollingSpeed.y);

	output.texCoord3 = (input.tex * scales.z);
	output.texCoord3.y = output.texCoord3.y + (frameTime * scrollingSpeed.z);

	return output;
}