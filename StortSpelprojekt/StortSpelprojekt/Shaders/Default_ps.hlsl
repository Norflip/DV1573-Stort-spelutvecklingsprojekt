#include "PhongShading.hlsl"
#include "IO.hlsl"

#include "PSBlendModes.hlsl"
Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);
TextureCube skymap : register(t2); //just testing
SamplerState defaultSampleType : register (s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 normalmap = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if (hasAlbedo)
		textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

	if (hasNormalMap)
	{
		normalmap = normalMap.Sample(defaultSampleType, input.uv);
		input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
	}

	float3 normalized = normalize(input.normal);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

   

	uint2 tileIndex = uint2(floor(input.position.xy / (BLOCK_SIZE * BLOCK_SIZE)));

	uint startOffset = LightGrid[tileIndex].x;
	uint lightCount = LightGrid[tileIndex].y;

	for (uint i = 0; i < lightCount; i++)
	{
		float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
		uint lightIndex = LightIndexList[startOffset + i];
		Light light = Lights[lightIndex];
		
        float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		switch (light.type)
		{
		case DIRECTIONAL_LIGHT:
		{
			result = CalculateDirectionalLight(light, normalized, viewDirection);

		}
		break;
		case POINT_LIGHT:
		{
			result = CalculatePointLight(light, normalized, input.worldPosition, viewDirection);
		}
		break;
		case SPOT_LIGHT:
		{
			result = CalculateSpotLight(light, normalized, input.worldPosition, viewDirection);
		}
		break;
		}
		finalColor += result;
	}

	finalColor = saturate(finalColor * textureColor);
	finalColor.a = 1.0f;

	//return float4(input.normal, 1.0f);
	return finalColor; 
}
