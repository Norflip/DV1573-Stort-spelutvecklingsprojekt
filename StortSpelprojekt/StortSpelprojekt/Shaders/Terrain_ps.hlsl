
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
	float channel = testTexture.Sample(m_samplerState, input.uv).x;
	return float4(channel, channel, channel, 1.0f);
}
