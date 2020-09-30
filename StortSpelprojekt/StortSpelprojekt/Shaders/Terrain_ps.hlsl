
struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

Texture2D testTexture : register (t0);
SamplerState m_samplerState : register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float3 testSunDirection = -normalize(float3(1,-1,0));

	float d = dot(input.normal, testSunDirection);
	return float4(d, d, d, 0);
}
