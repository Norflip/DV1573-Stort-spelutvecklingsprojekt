
#include "PhongShading.hlsl"

#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{
	
    float4 textureColor = input.colour;
   
   
	float3 normalized = normalize(input.normal);
	float3 viewDirection = cameraPosition - input.posToEye;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0);


	uint2 tileIndex = uint2(floor(input.position.xy / (BLOCK_SIZE)));

	uint startOffset = LightGrid[tileIndex].x;
	uint lightCount = LightGrid[tileIndex].y;

	finalColor = IterateLights(startOffset, lightCount, finalColor, normalized, input.posToEye, viewDirection);
    //textureColor *= float4(0.4, 0.4, 0.4, 1.0);

    finalColor*= textureColor;
    return finalColor;
	
	
}