#include "CommonBuffers.hlsl"
#include "IO.hlsl"

Texture2D grassHeightMap : register (t0);
Texture2D grassColorMap : register (t1);
Texture2D noiseMap : register (t2);
SamplerState LinearSampler : register(s0);






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

	float3 position1 = grassStraws[v1].position;
	float3 position2 = grassStraws[v2].position;
	float3 position3 = grassStraws[v3].position;

	float3 pos = BCC.x * position1 + BCC.y * position2 + BCC.z * position3;

	float3 normal1 = grassStraws[v1].normal;
	float3 normal2 = grassStraws[v2].normal;
	float3 normal3 = grassStraws[v3].normal;

	
	float3 normal = BCC.x * normal1 + BCC.y * normal2 + BCC.z * normal3;

	float2 uv1 = MasterHairStrands[v1].uv;
	float2 uv2 = MasterHairStrands[v2].uv;
	float2 uv3 = MasterHairStrands[v3].uv;

	
	float2 uvPlane = BCC.x * uv1 + BCC.y * uv2 + BCC.z * uv3;

	
	float4 colour = grassColorMap.SampleLevel(LinearSampler, uvPlane, 0);


	output.bladeHeight = grassHeightMap.SampleLevel(LinearSampler, uvPlane, 0).r * 2.5f/*length*/;
	output.height = uv.x * output.bladeHeight;

	output.expandVector = normalize(pos - position1);

	
	pos.xyz += normal * output.height;

	float noiseSample = noiseMap.SampleLevel(LinearSampler, 4 * uvPlane, 0).r;

	float disp = 1.5f /*width*/ * pow(uv.x, 2) * (noiseSample + 0.5 * abs(sin(time + noiseSample)));

	output.position = float4(pos.xyz, 1.0f);
	output.displacement = float3(0, -disp, 0);
	output.tex.y = uv.x;
	output.normal = mul(float4(normal, 0), world).xyz;

	output.colour = float4(colour.rgb, 1.0f);

	return output;
}
