#include "IO.hlsl"
TextureCube skymap;

float4 main(VS_OUTPUT_SKY input) : SV_TARGET
{	
	return float4(1,0,0,1);
}