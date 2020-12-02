#include "CommonBuffers.hlsl"
#include "IO.hlsl"

struct GS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float3 worldPosition : POSITION;
};

float4 main(GS_OUTPUT input) : SV_TARGET
{
	return  float4(1,1,1,1) * 0.5f;
}