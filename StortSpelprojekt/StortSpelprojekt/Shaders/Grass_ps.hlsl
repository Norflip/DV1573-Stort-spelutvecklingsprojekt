#include "CommonBuffers.hlsl"
#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{

	float3 colour = input.colour.rgb;
	colour *= float3(0.4, 0.4, 0.4);
	return float4(colour,1);
}