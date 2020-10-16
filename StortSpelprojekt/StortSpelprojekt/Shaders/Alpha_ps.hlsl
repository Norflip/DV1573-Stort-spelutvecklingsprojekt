#include "PhongShading.hlsl"
#include "PSBlendModes.hlsl"
#include "IO.hlsl"
Texture2D diffuseMap : register (t0);
Texture2D alphaMap : register (t1); //Maybe you can do this directly inside an alpha channel.
TextureCube skymap : register(t2);
SamplerState defaultSampleType : register (s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

	float3 normalized = normalize(input.normal);

	return float4(normalized, 1.0f);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

	finalColor += CalculateDirectionalLight(sunDirection, normalized, viewDirection);

	for (int i = 0; i < nrOfPointLights; i++)
	{
		finalColor += CalculatePointLight(pointLights[i], normalized, input.worldPosition, viewDirection);
	}

	textureColor.a = alphaMap.Sample(defaultSampleType, input.uv).r;

	finalColor = saturate(finalColor * textureColor);

	
	return finalColor;
}