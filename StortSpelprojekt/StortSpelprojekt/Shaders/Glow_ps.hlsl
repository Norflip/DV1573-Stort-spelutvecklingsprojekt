#include "CommonBuffers.hlsl"

Texture2D rgbTexture : register(t0);
Texture2D glowTexture : register(t2);
Texture2D emssiveTexture : register(t3);


SamplerState glowSampler;
#include "IO.hlsl"

struct PS_INPUT_GLOW
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT_GLOW input) : SV_TARGET
{
    float4 albedo = rgbTexture.Sample(glowSampler, input.uv);
    float4 glow = glowTexture.Sample(glowSampler, input.uv);
    float4 emissive = emssiveTexture.Sample(glowSampler, input.uv);
       
    return albedo + emissive + glow;
}