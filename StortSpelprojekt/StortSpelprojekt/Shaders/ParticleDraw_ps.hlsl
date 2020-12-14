SamplerState samLinear : register(s0);
Texture2D tex  : register(t0);

cbuffer cbPerFrame : register(b0)
{
	float4 particleColor;
	float3 eyePosW;
	float3 emitPosW;
	float3 emitDirW;
	float gameTime;
	float timeStep;
	float particleMaxAge;
	row_major matrix viewProj;
	bool usingTexture;
	float3 particleSpreadMulti;
	int particlesPerSecond;
	float2 particleSize;
};

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
};

float4 main(GeoOut pin) : SV_TARGET
{
	if (usingTexture)
	{
		float4 color = tex.Sample(samLinear, pin.Tex);
		pin.Color *= color;
	}
	return  pin.Color;
}