//Texture2D screenTexture : register (t0);
//SamplerState defaultSampleType : register (s0);

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

float RgbToLuma(float3 color)
{
	return sqrt(dot(color), float3(0.299, 0.587, 0.114));
}

float4 main() : SV_TARGET
{
	float3 color = screenTexture.sample(defaultSampleType, input.uv).rgb;

	float lumaCenter = RgbToLume(color);

	float lumaDown = RgbToLuma(textureOffset(screenTexture, input.uv, float2(0, -1)).rgb);

	return float4(color, 1.0f);
}