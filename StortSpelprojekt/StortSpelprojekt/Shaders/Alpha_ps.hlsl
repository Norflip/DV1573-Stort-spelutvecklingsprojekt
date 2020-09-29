#include "PhongShading.hlsl"
#include "PSBlendModes.hlsl"
Texture2D diffuseMap : register (t0);
Texture2D alphaMap : register (t1); //Maybe you can do this directly inside an alpha channel.
TextureCube skymap : register(t2);
SamplerState defaultSampleType : register (s0);

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	
	

	
	float4 textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

	

	float3 normalized = normalize(input.normal);

	float3 viewDirection = cameraPosition - input.worldPosition;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0);

	finalColor += CalculateDirectionalLight(sunDirection, normalized, viewDirection);

	for (int i = 0; i < nrOfPointLights; i++)
	{
		finalColor += CalculatePointLight(pointLights[i], normalized, input.worldPosition, viewDirection);
	}

	textureColor.a = alphaMap.Sample(defaultSampleType, input.uv).r;

	/*Testing something*/

	/*float3 reflectVector = reflect(-viewDirection, input.normal);

	float3 envColor = skymap.SampleLevel(defaultSampleType, reflectVector, 0).rgb;

	float4 combColor = saturate(finalColor * textureColor *(float4(envColor, 1) * 10));

	float3 blue = float3(0, 0, 1);

	float hue = GetH(blue);

	float sat = GetS(blue);

	float lum = GetL(blue);






	float3 testColor = PSColorMode(combColor.xyz, hue, sat, lum, 0.5);


	return float4(testColor.rgb, textureColor.a);*/

	/************/





	finalColor = saturate(finalColor * textureColor);


	float3 reflectVector = reflect(-viewDirection, input.normal);

	float4 envColor = skymap.SampleLevel(defaultSampleType, reflectVector, 0);

	envColor *= 1;

	float hue = GetH(finalColor.rgb);

	float sat = GetS(finalColor.rgb);

	float lum = GetL(finalColor.rgb);

	return float4(PSColorMode(envColor.rgb, hue, sat, lum, 0.5), textureColor.a);


	//return float4(input.normal, 1.0f);
	return finalColor;
}