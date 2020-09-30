#include "IO.hlsl"

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT_QUAD main(VertexInputType input)
{
	VS_OUTPUT_QUAD output;
	output.position = input.position;
	output.uv = input.uv;

	return output;
}