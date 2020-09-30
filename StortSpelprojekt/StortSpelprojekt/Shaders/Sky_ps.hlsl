//#include "CommonBuffers.hlsl"
//#include "PSBlendModes.hlsl"
Texture2D diffuse : register(t0);
TextureCube skymap : register(t2);
SamplerState defaultSampler : register (s0);
#include "IO.hlsl"
TextureCube skymap;


float4 main(VS_OUTPUT_SKY input) : SV_TARGET
{
	return float4(1,0,0,1);
}