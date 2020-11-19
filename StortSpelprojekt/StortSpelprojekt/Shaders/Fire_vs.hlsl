#include "CommonBuffers.hlsl"
#include "IO.hlsl"

cbuffer NoiseBuffer : register(b1)
{
	float frameTime;
	float3 scrollingSpeed;
	float3 scales;
	float padding;
};

//struct VertexInput
//{
//	float4 position : POSITION;
//	float3 tex : TEXCOORD0;
//};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;	
};

VertexOutput main(VS_INPUT input)
{
	VertexOutput output = (VertexOutput)0;

	input.position.w = 1.0f;
	output.position = mul(mvp, input.position);

	output.tex = input.uv;

	output.texCoord1 = (input.uv * scales.x);
	output.texCoord1.y = output.texCoord1.y + (frameTime * scrollingSpeed.x);

	output.texCoord2 = (input.uv * scales.y);
	output.texCoord2.y = output.texCoord2.y + (frameTime * scrollingSpeed.y);

	output.texCoord3 = (input.uv * scales.z);
	output.texCoord3.y = output.texCoord3.y + (frameTime * scrollingSpeed.z);

	return output;
}