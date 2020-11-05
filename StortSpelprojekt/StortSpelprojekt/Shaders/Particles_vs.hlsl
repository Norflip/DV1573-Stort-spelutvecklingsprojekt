//#include "CommonBuffers.hlsl"
//#include "IO.hlsl"

cbuffer MatrixBuffer : register(b0)
{
	row_major matrix worldViewProjection;
	row_major matrix worldspace;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WPOSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

VertexOutput main (VertexInput input)
{
	VertexOutput output = (VertexOutput)0;

	input.position.w = 1.0f;
	output.position = mul(worldViewProjection, input.position);
	output.worldPosition = mul(worldspace, input.position);
	output.uv = input.uv;

	// Store the particle color for the pixel shader. // We dont use it but we can if we want to.
	output.color = input.color;

	return output;
}