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

	finalColor += CalculateDirectionalLight(sunDirection, normalized, viewDirection);

	for (int i = 0; i < nrOfPointLights; i++)
	{
		finalColor += CalculatePointLight(pointLights[i], normalized, input.worldPosition, viewDirection);
	}

	finalColor = saturate(finalColor * textureColor);


	/*Testing something*/

	/*float3 reflectVector = reflect(-viewDirection, input.normal);

	float3 envColor = skymap.SampleLevel(defaultSampleType, reflectVector, 0).rgb;

	float4 combColor = saturate(finalColor * textureColor* (float4(envColor, 1) * 10));
	float3 blue = float3(0, 0, 1);

	float hue = GetH(blue);

	float sat = GetS(blue);

	float lum = GetL(blue);

	

	

	
	float3 testColor =PSColorMode(combColor.xyz, hue, sat, lum,0.5);

	
	return float4(testColor.rgb, 1);
	
	
	return float4(Overlay(float3(1, 0, 0),finalColor.rgb ), 1);


	return float4(Screen(float3(1, 0, 0), finalColor.rgb), 1);

	*/
	
	
	finalColor = saturate(finalColor * textureColor);


	float3 reflectVector = reflect(-viewDirection, input.normal);

	float4 envColor = skymap.SampleLevel(defaultSampleType, reflectVector, 0);

	envColor *= 1;

	float hue = GetH(finalColor.rgb);

	float sat = GetS(finalColor.rgb);

	float lum = GetL(finalColor.rgb);

	return float4(PSColorMode(envColor.rgb,hue, sat, lum, 0.5), 1);

	//return float4(input.normal, 1.0f);
	return finalColor; //  float4(finalColor.r, finalColor.g, finalColor.b * 5, finalColor.a);
}
