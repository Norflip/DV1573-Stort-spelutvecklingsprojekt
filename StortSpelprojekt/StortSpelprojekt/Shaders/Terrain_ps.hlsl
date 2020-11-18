#include "PhongShading.hlsl"
#include "IO.hlsl"

float invLerp(float from, float to, float value) 
{
	return (value - from) / (to - from);
}

float remap(float origFrom, float origTo, float targetFrom, float targetTo, float value) 
{
	float rel = invLerp(origFrom, origTo, value);
	return lerp(targetFrom, targetTo, rel);
}

Texture2D testTexture : register (t0);
Texture2D grassTexture : register (t1);
Texture2D roadTexture : register (t2);

SamplerState m_dataSamplerState : register(s0);
SamplerState m_textureSamplerState : register(s1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	const float UV_SCALE = 8.0f;

	float4 data = testTexture.Sample(m_dataSamplerState, input.uv);

	float4 grass = grassTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);
	float4 road = roadTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);

	//return float4(data.z, data.z, data.z, 1.0f);
	
	float t = smoothstep(0.2f, 0.8f, data.z);
	float4 textureColor = lerp(road, grass, t);
	textureColor.a = 1.0f;

	//return d;

	float3 viewDirection = cameraPosition - input.worldPosition;
	float4 finalColor = float4(0,0,0,1);

	float3 normal = normalize(input.normal);
    

	uint2 tileIndex = uint2(floor(input.position.xy / (BLOCK_SIZE * BLOCK_SIZE)));

	uint startOffset = LightGrid[tileIndex].x;
	uint lightCount = LightGrid[tileIndex].y;

	for (uint i = 0; i < lightCount; i++)
	{
		uint lightIndex = LightIndexList[startOffset + i];
		Light light = Lights[lightIndex];
		
        float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		switch (light.type)
		{
		case DIRECTIONAL_LIGHT:
		{
			result = CalculateDirectionalLight(light, normal, viewDirection);
		}
		break;
		case POINT_LIGHT:
		{
			result = CalculatePointLight(light, normal, input.worldPosition, viewDirection);
		}
		break;
		case SPOT_LIGHT:
		{
			result = CalculateSpotLight(light, normal, input.worldPosition, viewDirection);
		}
		break;
		}
		finalColor += result;
	}
	finalColor = saturate(textureColor * finalColor);
	finalColor.a = 1.0f;

	//return textureColor;
	//return float4(normal, 1.0f);
	return finalColor;
}
