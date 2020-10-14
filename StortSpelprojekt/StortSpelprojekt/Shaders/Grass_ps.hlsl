#include "CommonBuffers.hlsl"
#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{

	float3 colour = input.colour.rgb;

	return float4(colour,1/*input.fade*/);
}