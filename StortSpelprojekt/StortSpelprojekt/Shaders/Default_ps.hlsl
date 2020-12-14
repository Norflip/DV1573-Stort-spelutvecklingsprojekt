#include "PhongShading.hlsl"
#include "IO.hlsl"

#include "PSBlendModes.hlsl"
Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);
Texture2D emissiveTexture : register(t2);
//TextureCube skymap : register(t2); //just testing
SamplerState defaultSampleType : register (s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{	
	float4 textureColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 normalmap = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	if (hasAlbedo)
		textureColor = diffuseMap.Sample(defaultSampleType, input.uv);
	
	if (hasNormalMap)
	{
		normalmap = normalMap.Sample(defaultSampleType, input.uv);
		input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
	}

	if(hasEmissiveMap)
        emissiveColor = emissiveTexture.Sample(defaultSampleType, input.uv);
		
	
	float3 normalized = normalize(input.normal);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

    uint2 tileIndex = uint2(floor(input.position.xy / (BLOCK_SIZE)));

	uint startOffset = LightGrid[tileIndex].x;
	uint lightCount = LightGrid[tileIndex].y;

	finalColor = IterateLights(startOffset, lightCount, finalColor, normalized, input.worldPosition, viewDirection);

	finalColor = saturate(finalColor * textureColor);
	finalColor.a = 1.0f;

	//return float4(input.normal, 1.0f);
    return finalColor + emissiveColor;
    //return emissiveColor;
}
