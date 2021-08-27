Texture2D/*MS*/<float4> texture : register (t0);
Texture2D/*MS*/<float4> glowTexture : register(t2);

RWTexture2D<float4> outTexture : register(u0);


[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
    uint2 currentPixel = uint2(DTid.xy);
    int range = 5;
    int2 direction = int2(-1, 0);
    float4 blurrColor = float4(0, 0, 0, 0);
    float weights[5] = (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    
    for (int i = -range; i <= range; i++)
    {
        uint weightIndex = (uint) abs(i);
        blurrColor += weights[weightIndex] * glowTexture[mad(i, direction, currentPixel)];
    }
    //direction = int2(0, 1);
    //for (int i = -range; i <= range; i++)
    //{
    //    uint weightIndex = (uint) abs(i);
    //    blurrColor += weights[weightIndex] * glowTexture[mad(i, direction, currentPixel)];
    //}
    
    outTexture[currentPixel] = blurrColor;
    
}