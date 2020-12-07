#include "IO.hlsl"

Texture2D emissiveTexture : register(t2);
SamplerState emissiveSampler;

struct Emissive_PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    emissiveColor = emissiveTexture.Sample(emissiveSampler, input.uv);
    
    float4 finalEmissiveColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    return emissiveColor;
}