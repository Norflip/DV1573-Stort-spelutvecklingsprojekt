#include "CommonBuffers.hlsl"
#include "IO.hlsl"




VS_OUTPUT_GRASS main(VS_INPUT input)
{
	VS_OUTPUT_GRASS output;

	

	output.uv = float2(0,0);
	output.worldPosition = mul(input.position, mvp).xyz;
	
	output.position = mul(mvp, input.position);
	
	float d = distance(output.worldPosition, cameraPosition);

	float tess = saturate((50.0f - d) / (50.0f - 1.0f));

	//output.tessFactor = 0.0f + tess * (2.0f - 0.0f);

	output.tessFactor = 1.0f + tess * (1.0f - 1.0f);
	
	return output;
}