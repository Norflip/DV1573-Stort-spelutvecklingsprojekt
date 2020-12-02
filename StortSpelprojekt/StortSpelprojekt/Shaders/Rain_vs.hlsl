#include "CommonBuffers.hlsl"

struct VS_INPUT_PARTICLE
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main (VS_INPUT_PARTICLE input)
{
	VS_OUTPUT output;
	output.position = input.position;// mul(input.position, world);
	return output;
}