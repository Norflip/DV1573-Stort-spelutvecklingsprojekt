#include "CommonBuffers.hlsl"
#include "IO.hlsl"




Texture2D grassHeightMap : register (t1);
SamplerState linearSampler : register(s0);

//PatchTess PatchHS(InputPatch<VS_CONTROL_POINT_OUTPUT, 3> patch, uint patchID : SV_PrimitiveID)

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT_GRASS hsPerIsoLinePatch(InputPatch<VS_OUTPUT_GRASS,1> input, uint i : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT_GRASS output;



	output.edgeTesselation[1] = (uint)input[0].tessFactor * fmod(1, 64); //4
	output.edgeTesselation[0] = (uint)input[0].tessFactor * fmod(1, 64); //max times max per triangle fmod
	
	

	output.triangleIndex = i;

	uint v1 = (uint)grassIndices[i * 3].x;
	uint v2 = (uint)grassIndices[i * 3 + 1].x;
	uint v3 = (uint)grassIndices[i * 3 + 2].x;

	float3 normal1 = grassStraws[v1].normal.xyz;
	float3 normal2 = grassStraws[v2].normal.xyz;
	float3 normal3 = grassStraws[v3].normal.xyz;

	float3 normal = (normal1 + normal2 + normal2) / 3.0f;
	float3 viewNormal = mul(float4(normal, 0.0f), wv).xyz;

	if (viewNormal.z >= 0.3)
	{
		output.edgeTesselation[0] = 0;
	}

	float2 uv1 = grassStraws[v1].uv.xy;
	float2 uv2 = grassStraws[v2].uv.xy;
	float2 uv3 = grassStraws[v3].uv.xy;

	float2 uv = (uv1 + uv2 + uv3) / 3.0f;

	float len = grassHeightMap.SampleLevel(linearSampler, uv, 0).r;

	if (len < 0.1)
	{
		output.edgeTesselation[0] = 0;
	}

	return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(1)]
[patchconstantfunc("hsPerIsoLinePatch")]
HS_OUTPUT_GRASS main(InputPatch<VS_OUTPUT_GRASS, 1> p,
	uint i : SV_OutputControlPointID)
{
	HS_OUTPUT_GRASS output = (HS_OUTPUT_GRASS)0;
	output.position = p[i].worldPosition;
	output.tex = p[i].uv;
	return output;
}

