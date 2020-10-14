#include "IO.hlsl"
Texture2D diffuseMaps : register (t0);
Texture2D normalMaps : register (t1);

SamplerState defaultSampleType : register (s0);

float4 main(VS_OUTPUT_QUAD input) : SV_TARGET
{
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    diffuseColor = diffuseMaps.Sample(defaultSampleType, input.uv);

    return diffuseColor; 
}