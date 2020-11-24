
#include "PhongShading.hlsl"

#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{
    Material mat;
    mat.matAmbient = float4(0.5f, 0.5f, 0.5f, 1);
    mat.matDiffuse = float4(0.5f, 0.5f, 0.5f, 1);
    mat.matSpecular = float4(0.1f, 0.1f, 0.1f, 0.1f);
    mat.hasAlbedo = false;
    mat.hasNormalMap = false;
    
    float4 textureColor = input.colour;
    //float4 textureColor = float4(0.05, 0.2, 0, 1); //
    
    
	//float3 colour = input.colour.rgb;
	//colour *= float3(0.4, 0.4, 0.4);
	//return float4(colour,1);
    
 
    //float4 normalmap = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //input.normal = float4(0.2f, 0.2f, 0.2f, 0.2f);
    //if (hasAlbedo)
        //textureColor = mat.diffuseMap.Sample(defaultSampleType, input.tex);
    
    //if (hasNormalMap)
    //{
    //    normalmap = normalMap.Sample(defaultSampleType, input.uv);
    //    input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
    //}
    float4 worldPosition = float4(input.posToEye, 1); //mul(input.position, world);
    //float4 viewPosition = mul(input.position, view);
    
    //float3 normalized = normalize(input.normal);
    
    float3 normalized = input.normal;//  -float3(0, 0.8, 0);
    float3 viewDirection = cameraPosition - worldPosition.xyz;
  
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

    
    
    
    uint2 tileIndex = uint2(floor(input.position.xy / (BLOCK_SIZE)));

    uint startOffset = LightGrid[tileIndex].x;
    uint lightCount = LightGrid[tileIndex].y;

    finalColor = IterateLights(mat, startOffset, lightCount, finalColor, normalized, worldPosition.xyz, viewDirection);
    finalColor = saturate(finalColor * textureColor);
    finalColor.a = 1.0f;


    return finalColor;
	
    //return normalize(float4(input.normal, 1));
}