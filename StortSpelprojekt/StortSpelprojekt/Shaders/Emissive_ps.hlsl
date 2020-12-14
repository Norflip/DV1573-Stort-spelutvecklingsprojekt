#include "IO.hlsl"

Texture2D emissiveTexture : register(t2);
SamplerState emissiveSampler;

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    emissiveColor = emissiveTexture.Sample(emissiveSampler, input.uv);
 
    //if(emissiveColor.r <= 0)
    //{
    //    discard;
    //}
    
    return emissiveColor;
    //return float4(0, 0, 0, 0);
}