#include "PhongShading.hlsl"
#include "IO.hlsl"

#include "PSBlendModes.hlsl"
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
TextureCube skymap : register(t2); //just testing
SamplerState defaultSampleType : register(s0);


//StructuredBuffer<uint> LightIndexList : register(t9);
//Texture2D<uint2> LightGrid : register(t10);

[earlydepthstencil]
float4 main(VS_OUTPUT input) : SV_TARGET //VS_OUTPUT input = IN
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
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Get the index of the current pixel in the light grid.
    uint2 tileIndex = uint2(floor(input.position.xy / BLOCK_SIZE));

	// Get the start position and offset of the light in the light index list.
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
                    result = CalculateDirectionalLight(light, normalized, viewDirection);
				//DirectionVS =sunDirection
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

    return finalColor;
}

////for_alpha_ps.hlsl

//#include "PhongShading.hlsl"
//#include "PSBlendModes.hlsl"
//#include "IO.hlsl"
//Texture2D diffuseMap : register(t0);
//Texture2D normalMap : register(t1);
//TextureCube skymap : register(t2);
Texture2D alphaMap : register(t3);
//SamplerState defaultSampleType : register(s0);
//StructuredBuffer<uint> LightIndexList : register(t9);
//Texture2D<uint2> LightGrid : register(t10);

[earlydepthstencil]
float4 mainAlpha(VS_OUTPUT input) : SV_TARGET //VS_OUTPUT input = IN
{
    float4 textureColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
    float4 normalmap = float4(1.0f, 1.0f, 1.0f, 1.0f);

    //if (hasAlbedo)
    //    textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

    if (hasNormalMap)
    {
        normalmap = normalMap.Sample(defaultSampleType, input.uv);
        input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
    }

    float3 normalized = normalize(input.normal);
    float3 viewDirection = cameraPosition - input.worldPosition;
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Get the index of the current pixel in the light grid. //clip space position
    uint2 tileIndex = uint2(floor(input.position.xy / BLOCK_SIZE));

	// Get the start position and offset of the light in the light index list.
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
                result = CalculateDirectionalLight(light, normalized, viewDirection);
				//DirectionVS =sunDirection
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

    textureColor.a = alphaMap.Sample(defaultSampleType, input.uv).r;
    finalColor = saturate(finalColor * textureColor);
    
    if (finalColor.a <= 0.0f)
        discard;
    
    finalColor.a = 1.0f;

    return finalColor;
}

//for Terrain_ps.hlsl
//#include "PhongShading.hlsl"
//#include "IO.hlsl"

float invLerp(float from, float to, float value)
{
    return (value - from) / (to - from);
}

float remap(float origFrom, float origTo, float targetFrom, float targetTo, float value)
{
    float rel = invLerp(origFrom, origTo, value);
    return lerp(targetFrom, targetTo, rel);
}

Texture2D testTexture : register(t0);
Texture2D grassTexture : register(t1);
Texture2D roadTexture : register(t2);

SamplerState m_dataSamplerState : register(s0);
SamplerState m_textureSamplerState : register(s1);

[earlydepthstencil]
float4 mainTerrain(VS_OUTPUT input) : SV_TARGET //VS_OUTPUT input = IN
{
    const float UV_SCALE = 8.0f;

    float4 data = testTexture.Sample(m_dataSamplerState, input.uv);

    float4 grass = grassTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);
    float4 road = roadTexture.Sample(m_textureSamplerState, input.uv * UV_SCALE);
    
    
    
    float t = smoothstep(0.2f, 0.8f, data.z);
    float4 textureColor = lerp(road, grass, t);
    textureColor.a = 1.0f;
    
    float3 normalized = normalize(input.normal);
    float3 viewDirection = cameraPosition - input.worldPosition;
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Get the index of the current pixel in the light grid. //clip space position
    uint2 tileIndex = uint2(floor(input.position.xy / BLOCK_SIZE));

	// Get the start position and offset of the light in the light index list.
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
                    result = CalculateDirectionalLight(light, normalized, viewDirection);
				//DirectionVS =sunDirection
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


    finalColor = saturate(textureColor * textureColor);
    finalColor.a = 1.0f;

    return finalColor;
}