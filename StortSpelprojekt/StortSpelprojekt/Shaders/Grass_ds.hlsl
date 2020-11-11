#include "CommonBuffers.hlsl"
#include "IO.hlsl"

Texture2D grassHeightMap : register (t1);
Texture2D grassColorMap : register (t0);
Texture2D noiseMap : register (t2);
Texture2D chunkData : register (t6);

SamplerState LinearSampler : register(s0);

/*Alter the commented values to change the grass*/
[domain("isoline")]
DS_OUTPUT_GRASS main(HS_CONSTANT_DATA_OUTPUT_GRASS input,
	OutputPatch<HS_OUTPUT_GRASS, 1> op,
	float2 uv : SV_DomainLocation)
{
	DS_OUTPUT_GRASS output = (DS_OUTPUT_GRASS)0;

	uint currentStrand = (int)(uv.y * input.edgeTesselation[0] + 0.5);

	float3 BCC = BCCoordinates[currentStrand].xyz;

	uint index = input.triangleIndex * 3;
	uint v1 = (uint)grassIndices[index].x;
	uint v2 = (uint)grassIndices[index + 1].x;
	uint v3 = (uint)grassIndices[index + 2].x;

	float2 uv1 = grassStraws[v1].uv.xy;
	float2 uv2 = grassStraws[v2].uv.xy;
	float2 uv3 = grassStraws[v3].uv.xy;

	float2 uvPlane = BCC.x * uv1 + BCC.y * uv2 + BCC.z * uv3;

    const float TERRAIN_HEIGHT = 7.0f;
	
	float h = chunkData.SampleLevel(LinearSampler, uvPlane, 0).r;
    float y = h * TERRAIN_HEIGHT;

	float3 position1 = grassStraws[v1].position.xyz;
	float3 position2 = grassStraws[v2].position.xyz;
	float3 position3 = grassStraws[v3].position.xyz;

	position1.y += y;
	position2.y += y;
	position3.y += y;

	float3 pos = BCC.x * position1 + BCC.y * position2 + BCC.z * position3;

	float3 normal1 = grassStraws[v1].normal.xyz;
	float3 normal2 = grassStraws[v2].normal.xyz;
	float3 normal3 = grassStraws[v3].normal.xyz;

	float3 normal = BCC.x * normal1 + BCC.y * normal2 + BCC.z * normal3;

	float4 colour = grassColorMap.SampleLevel(LinearSampler, uvPlane, 0);

	output.bladeHeight = grassHeightMap.SampleLevel(LinearSampler, uvPlane, 0).r * /*grassRadius*/1;
	output.height = uv.x * output.bladeHeight;

	output.expandVector = normalize(pos - position1);

	pos.xyz += normal * output.height;

	float noiseSample = noiseMap.SampleLevel(LinearSampler, 4 * uvPlane, 0).r;

	float disp = /*grassDisplacement*/ 0.05f * pow(uv.x, 1.0); //grassDisplacement == Length
	float dispT = /*grassDisplacement*/0.05f * pow(uv.x, 1.0) * (noiseSample + 2.5 * abs(sin((time * 1) + noiseSample)));

	//dispT *= 0.1;

	output.position = float4(pos.x, pos.y - 0.4f, pos.z, 1.0f);
	output.displacement = float3(dispT, disp, dispT);
	output.tex.y = uv.x;
	output.normal = mul(float4(normal, 0), world);

	output.colour = float4(colour.rgb, 1.0f);

	return output;
}
