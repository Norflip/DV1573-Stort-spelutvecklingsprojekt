#include "CommonBuffers.hlsl"
#include "IO.hlsl"



VS_OUTPUT_GRASS main(VS_INPUT input)
{
	VS_OUTPUT_GRASS output;


	output.uv = float2(0,0);
	output.worldPosition = mul(world, input.position).xyz;
	
	output.position = mul(mvp, input.position);
	
	float d = distance(output.worldPosition, cameraPosition);

	float tess = saturate((10.0f - d) / (10.0f - 200.0f));

	output.tessFactor = 0.0f + tess * (3.0f - 0.0f);


	return output;
}