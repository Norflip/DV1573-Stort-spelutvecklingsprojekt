#include "PhongShading.hlsl"
#include "IO.hlsl"

#include "PSBlendModes.hlsl"
Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
TextureCube skymap : register(t2); //just testing
SamplerState defaultSampleType : register(s0);


StructuredBuffer<uint> LightIndexList : register(t9);
Texture2D<uint2> LightGrid : register(t10);

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
                    result = CalculateDirectionalLight(sunDirection, normalized, viewDirection);
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
                    result = CalculateDirectionalLight(sunDirection, normalized, viewDirection);
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