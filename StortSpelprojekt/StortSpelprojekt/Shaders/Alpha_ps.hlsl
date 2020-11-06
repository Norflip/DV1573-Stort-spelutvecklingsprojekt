#include "PhongShading.hlsl"
#include "PSBlendModes.hlsl"
#include "IO.hlsl"
Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);
TextureCube skymap : register(t2);
Texture2D alphaMap : register (t3);
SamplerState defaultSampleType : register (s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor = diffuseMap.Sample(defaultSampleType, input.uv);
	if (hasNormalMap)
	{
		float4 normalmap = normalMap.Sample(defaultSampleType, input.uv);
		input.normal = CalculateNormalMapping(input.normal, input.tangent, normalmap);
	}
	float3 normalized = normalize(input.normal);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

	finalColor += CalculateDirectionalLight(sunDirection, normalized, viewDirection);

	for (int i = 0; i < nrOfPointLights; i++)
	{
		finalColor += CalculatePointLight(pointLights[i], normalized, input.worldPosition, viewDirection);
	}

	textureColor.a = alphaMap.Sample(defaultSampleType, input.uv).r;
	finalColor = saturate(finalColor * textureColor);
	
	if (finalColor.a <= 0.0f)
		discard;

	finalColor.a = 1.0f;
	return finalColor;

	//return float4(input.normal, 1.0f);
}