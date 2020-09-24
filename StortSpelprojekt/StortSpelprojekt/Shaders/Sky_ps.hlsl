TextureCube skymap;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 lPosition : LPOSITION;
	float2 uv : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{	
	return float4(1,0,0,1);
}