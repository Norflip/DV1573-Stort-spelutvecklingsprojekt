#include "CommonBuffers.hlsl"
#include "IO.hlsl"

Texture2D emissiveTexture : register(t0);
SamplerState emissiveSampler;

#define MAX_EMISSIVE_LENGTH 100

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // r�kna ut distansen fr�n camera position (skicka in p� n�got v�nster)
    // till input.worldPosition och spara den i alpha kanalen
    // s�tt en max_distance typ 100.0f och dela str�ckan med max_distance. (Det ger ett v�rde fr�n 0 till 1)
    // allt utanf�r 1 ska tas bort, discard;
    
    float l = length(cameraPosition - input.worldPosition);
    float l_01 = l / MAX_EMISSIVE_LENGTH;
    
   // if (saturate(l_01) != l_01)
    //    discard;
    
    float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    emissiveColor = emissiveTexture.Sample(emissiveSampler, input.uv);
    emissiveColor.a = l_01;
    return emissiveColor;
}