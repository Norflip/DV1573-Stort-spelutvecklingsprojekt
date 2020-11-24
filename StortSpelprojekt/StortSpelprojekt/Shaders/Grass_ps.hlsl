
#include "PhongShading.hlsl"

#include "IO.hlsl"

float4 main(GS_OUTPUT_GRASS input) : SV_TARGET
{
	
	//float3 colour = input.colour.rgb;
	//colour *= float3(0.4, 0.4, 0.4);
	//return float4(colour,1);

    float4 textureColor = input.colour;
   
    //float4 normalmap = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //input.normal = float4(0.2f, 0.2f, 0.2f, 0.2f);
    //if (hasAlbedo)
    //    textureColor = diffuseMap.Sample(defaultSampleType, input.uv);
    
    //if (hasNormalMap)
    //{
    //    normalmap = normalMap.Sample(defaultSampleType, input.uv);
    //    input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
    //}
  //  float4 worldPosition = mul(input.position, world);
  //  float4 viewPosition = mul(input.position, view);
    
  //  float3 normalized = normalize(input.normal);
  //  float3 viewDirection = cameraPosition - worldPosition.xyz;
  //  //float3 viewDirection = -input.posToEye;
   
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

  //  uint2 tileIndex = uint2(floor(viewPosition.xy / (BLOCK_SIZE)));

  //  uint startOffset = LightGrid[tileIndex].x;
  //  uint lightCount = LightGrid[tileIndex].y;

  //  for (uint i = 0; i < lightCount; i++)
  //  {
  //      uint lightIndex = LightIndexList[startOffset + i];
  //      Light light = Lights[lightIndex];
		
  //      float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
  //      switch (light.type)
  //      {
  //          case DIRECTIONAL_LIGHT:
		//{
  //                  result = CalculateDirectionalLight(light, normalized, viewDirection);

  //              }
  //              break;
  //          case POINT_LIGHT:
		//{
  //                  result = CalculatePointLight(light, normalized, worldPosition.xyz, viewDirection);
  //              }
  //              break;
  //          case SPOT_LIGHT:
		//{
  //                  result = CalculateSpotLight(light, normalized, viewPosition.xyz, viewDirection);
  //              }
  //              break;
  //      }
  //      finalColor += result;
  //  }

  //  finalColor = saturate(finalColor * textureColor);
  //  finalColor.a = 1.0f;

    //finalColor = float4(normalized, 1);
    //finalColor = matDiffuse;
    textureColor *= float4(0.4, 0.4, 0.4, 1.0);
    finalColor = textureColor;
    return finalColor;
	
	
}