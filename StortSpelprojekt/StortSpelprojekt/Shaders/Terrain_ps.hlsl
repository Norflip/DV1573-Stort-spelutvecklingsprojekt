float invLerp(float from, float to, float value) 
{
	return (value - from) / (to - from);
}

float remap(float origFrom, float origTo, float targetFrom, float targetTo, float value) 
{
	float rel = invLerp(origFrom, origTo, value);
	return lerp(targetFrom, targetTo, rel);
}


struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

Texture2D testTexture : register (t0);
Texture2D grassTexture : register (t1);
Texture2D roadTexture : register (t2);

SamplerState m_dataSamplerState : register(s0);
SamplerState m_textureSamplerState : register(s1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	const float UV_SCALE = 8.0f;

	float4 data = testTexture.Sample(m_dataSamplerState, input.uv);

	float4 grass = grassTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);
	float4 road = roadTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);

	float t = smoothstep(0.1f, 0.25f, data.z);
	float4 d = lerp(road, grass, t) * 0.4f;
	d.a = 1.0f;
	return d;
}
