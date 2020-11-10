//#include "CommonBuffers.hlsl"
//#include "IO.hlsl"

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : WPOSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

float4 main (PixelInput input) : SV_TARGET
{
    float4 textureColor = float4(0.0f,0.0f,0.0f,0.0f);
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.uv);

    // Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;

    return finalColor;
}
