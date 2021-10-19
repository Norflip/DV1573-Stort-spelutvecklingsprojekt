Texture2D<float4> glowTexture : register(t0);
RWTexture2D<float4> outTexture : register(u1);

[numthreads(8, 8, 1)]

void main( uint3 DTid : SV_DispatchThreadID)
{
    uint2 currentPixel = DTid.xy;

    float dst = glowTexture[currentPixel].a;
    
    const int range = max(1, 128 * (1.0f - dst));
    
    int2 direction = int2(1, 0);
    float4 blurrColor = float4(0, 0, 0, 0);
    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    for (int i = -range; i <= range; i++)
    {
        // HACK med mod%
        uint weightIndex = (uint) (abs(i) % 5);
        
        //int2 index = (direction * i) + int2(currentPixel);
        //int index = (direction.x * i) + int2(currentPixel.x);
        int2 index = (direction * i) + int2(currentPixel);
        blurrColor += weights[weightIndex] * glowTexture[index];
    }
    
    //AllMemoryBarrierWithGroupSync();
    
    direction = int2(0, 1);
    for (int j = -range; j <= range; j++)
    {
        uint weightIndex = (uint) (abs(j) % 5);
        //int2 index = (direction * i) + int2(currentPixel);
        //int index = (direction.y * j) + int2(currentPixel.y);
        int2 index = (direction * j) + int2(currentPixel);
        blurrColor += weights[weightIndex] * glowTexture[index];
    }
    
    //direction = int2(-1, 1);
    //for (int k = -range; k <= range; k++)
    //{
    //    uint weightIndex = (uint) (abs(k) % 5);
    //    int2 index = (direction * k) + int2(currentPixel);
    //    blurrColor += weights[weightIndex] * glowTexture[index];
    //}
    
    //direction = int2(1, 0);
    //for (int l = -range; l <= range; l++)
    //{
    //    uint weightIndex = (uint) (abs(l) % 5);
    //    int2 index = (direction * l) + int2(currentPixel);
    //    blurrColor += weights[weightIndex] * glowTexture[index];
    //}
    
    

    
    outTexture[currentPixel] = blurrColor;
}