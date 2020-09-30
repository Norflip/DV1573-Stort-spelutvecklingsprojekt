#include "CommonBuffers.hlsl"
#include "IO.hlsl"

VS_OUTPUT_SKY main (VS_INPUT input)
{
	VS_OUTPUT_SKY output;

	output.position = mul(mvp, input.position);
	output.position = output.position.xyww;		 // look at this later

	output.lPosition = mul(world, input.position);

	output.uv = input.uv;

	return output;
}