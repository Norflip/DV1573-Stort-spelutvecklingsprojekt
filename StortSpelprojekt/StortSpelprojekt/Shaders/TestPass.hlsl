Texture2D screenTexture : register (t0);
SamplerState defaultSampleType : register (s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main (PixelInputType input) : SV_TARGET
{
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
 
    if (input.uv.x < 0.5f)
        return float4(0, 1, 0, 1);

    diffuseColor = screenTexture.Sample(defaultSampleType, input.uv);

    return diffuseColor;
}