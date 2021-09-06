
Texture2D<float4> glowTexture : register(t0);
RWTexture2D<float4> outTexture : register(u1);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
    uint2 currentPixel = DTid.xy;


    const int range = 128;
    int2 direction = int2(1, 1);
    float4 blurrColor = float4(0, 0, 0, 0);
    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    
    [unroll]
    for (int i = -range; i <= range; i++)
    {
        // HACK med mod%
        uint weightIndex = (uint) (abs(i) % 5);
        
        int2 index = (direction * i) + int2(currentPixel);        
        blurrColor += weights[weightIndex] * glowTexture[index];
    }
    
    //direction = int2(0, 1);
    //for (int i = -range; i <= range; i++)
    //{
    //    uint weightIndex = (uint) abs(i);
    //    blurrColor += weights[weightIndex] * glowTexture[mad(i, direction, currentPixel)];
    //}
    
    outTexture[currentPixel] = blurrColor;
}