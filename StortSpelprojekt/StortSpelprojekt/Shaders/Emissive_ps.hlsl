#include "CommonBuffers.hlsl"
#include "IO.hlsl"

Texture2D emissiveTexture : register(t0);
SamplerState emissiveSampler;

#define MAX_EMISSIVE_LENGTH 100

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // räkna ut distansen från camera position (skicka in på något vänster)
    // till input.worldPosition och spara den i alpha kanalen
    // sätt en max_distance typ 100.0f och dela sträckan med max_distance. (Det ger ett värde från 0 till 1)
    // allt utanför 1 ska tas bort, discard;
    
    float l = length(cameraPosition - input.worldPosition);
    float l_01 = l / MAX_EMISSIVE_LENGTH;
    
   // if (saturate(l_01) != l_01)
    //    discard;
    
    float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    emissiveColor = emissiveTexture.Sample(emissiveSampler, input.uv);
    emissiveColor.a = l_01;
    return emissiveColor;
}