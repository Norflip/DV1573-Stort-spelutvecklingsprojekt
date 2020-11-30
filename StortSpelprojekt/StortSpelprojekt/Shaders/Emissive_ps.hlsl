#include "IO.hlsl"

Texture2D emissiveTexture : register(t2);
SamplerState emissiveSampler;

struct Emissive_PS_INPUT
{
    float2 uv : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 emissiveColor = 0;
    
    emissiveColor = emissiveTexture.Sample(emissiveSampler, input.uv);
    
    return emissiveColor;
}