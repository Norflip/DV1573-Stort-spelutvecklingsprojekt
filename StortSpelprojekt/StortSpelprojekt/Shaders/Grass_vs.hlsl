#include "CommonBuffers.hlsl"
#include "IO.hlsl"




VS_OUTPUT_GRASS main(VS_INPUT input)
{
	VS_OUTPUT_GRASS output;

	
	uint i = input.i;

	uint v1 = (uint)grassIndices[i * 3].x;
	uint v2 = (uint)grassIndices[i * 3 + 1].x;
	uint v3 = (uint)grassIndices[i * 3 + 2].x;


	float3 position1 = grassStraws[v1].position.xyz;
	float3 position2 = grassStraws[v2].position.xyz;
	float3 position3 = grassStraws[v3].position.xyz;


	float3 middle = lerp(position1, position2, 0.5f);

	float3 triPos = lerp(middle, position3, 0.5f);

	output.uv = float2(0, 0);

	output.position = input.position; //im not using this.
	output.worldPosition = mul(float4(triPos, 1), world).xyz;

	float d = distance(output.worldPosition, cameraPosition);

	float tess = saturate((50.0f - d) / (50.0f - 5.0f));

	output.tessFactor = 0.0f + tess * (2.0f - 0.0f);

	return output;
}