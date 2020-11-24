#include "CommonBuffers.hlsl"

// Calculate light with the given pointlight
float4 CalculatePointLight(Light light,Material mat, float3 normal, float3 objectPosition, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = light.lightPosition.xyz - objectPosition;
	
	float distance = length(lightVec);
	lightVec = normalize(lightVec);

	if (distance > light.range)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
    float attenuationFactor = clamp(1.0f - distance * distance / (light.range * light.range), 0.0f, 1.0f);

	float diffuseFactor = dot(lightVec, normal);
	if (diffuseFactor > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        float shine = pow(max(dot(reflection, viewDirection), 0.0f), 0.1f);

        ambient = saturate(mat.matAmbient * light.lightColor * attenuationFactor);
        diffuse = saturate(diffuseFactor * mat.matDiffuse * light.lightColor * attenuationFactor);
        specular = saturate(shine * mat.matSpecular * light.lightColor * attenuationFactor);
    }
	finalColor = ambient + diffuse + specular;
    return finalColor * light.intensity;
}

// Calculate normalmapping
float3 CalculateNormalMapping(float3 normal, float3 tangent, float4 normalmap)
{
	float3 mult = float3(1.0f, 1.0f, 1.0f);

	normalmap = (2.0f * normalmap) - 1.0f;

	// Scale the bump to your own preferences
	// Lower value will give more bump, while higher value will flatten
	normalmap.z = 1.0f;

	// If the tangent is a 0 vector, we need to calculate a new tangent
	if(all(tangent == float3(0.0f, 0.0f, 0.0f)))
	{
		// Create 2 new vectors which is orthogonal to the normal
		float3 temp1 = cross(normal, float3(0.0f, 0.0f, 1.0f));
		float3 temp2 = cross(normal, float3(0.0f, 1.0f, 0.0f));

		//Get their length
		float length1 = length(temp1);
		float length2 = length(temp2); 

		if (length1 > length2)
		{
			tangent = temp1;
		}
		else
		{
			tangent = temp2;
		}

		//Make sure tangent is completely orthogonal to normal
		tangent = normalize(tangent - dot(tangent, normal) * normal);
	}

	float3 biTangent = cross(tangent, normal);

	float3x3 TBN = float3x3(tangent, biTangent, normal);

	normal = mul(normalmap.xyz, TBN); //.xyz??

	return normal;
}
// Calculate light with the sun
float4 CalculateDirectionalLight(Light light, Material mat, float3 normal, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Can change from white to wanted color here
	//float4 sunColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float3 lightDirection = normalize(light.lightDirection);

    float diffuseFactor = max(dot(normal, -lightDirection), 0.0f);

	if (diffuseFactor > 0.0f)
	{
        float3 reflection = reflect(lightDirection, normal);
		float spec = pow(max(dot(viewDirection, reflection), 0.0f), 1.0f);

        ambient = saturate(matAmbient * light.lightColor );
        diffuse = diffuseFactor * light.lightColor * mat.matDiffuse;
        specular = spec * light.lightColor * mat.matSpecular;
    }

	finalColor = ambient + diffuse + specular;
	return finalColor * light.intensity;
}
float DoSpotCone(Light light, float3 L)
{
    float minCos = cos(radians(light.spotlightAngle));
    float maxCos = lerp(minCos, 1, 0.5f);
    //float3 lightDirectionVS = mul(float4(light.lightDirection, 0), transpose(view)).xyz;
    float cosAngle = dot(light.lightDirection, -L);
    return smoothstep(minCos, maxCos, cosAngle); //use clamp??
}

float4 CalculateSpotLight(Light light, Material mat, float3 normal, float3 objectPosition, float3 viewDirection)
{
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //viewDirection = mul(float4(viewDirection, 0), transpose(view)).xyz;
	
	//float3 lightPositionVS = mul(light.lightPosition, transpose(view));
    //float3 objVS = mul(float4(objectPosition, 1), transpose(view));
    float3 lightVec = light.lightPosition.xyz - objectPosition;
    //float3 lightVec = lightPositionVS - objVS;
    float distance = length(lightVec);
    lightVec = normalize(lightVec);

    if (distance > light.range)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float attenuationFactor = clamp(1.0f - distance * distance / (light.range * light.range), 0.0f, 1.0f);
	float spotIntensity = DoSpotCone(light, lightVec); //pointlight, light

    float diffuseFactor = dot(lightVec, normal);
    if (diffuseFactor > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        float shine = pow(max(dot(reflection, viewDirection), 0.0f), 0.1f); //matSpecular.w);

        ambient = saturate(mat.matAmbient * light.lightColor * attenuationFactor * spotIntensity);
        diffuse = saturate(diffuseFactor * mat.matDiffuse * light.lightColor * attenuationFactor * spotIntensity);
        specular = saturate(shine * mat.matSpecular * light.lightColor * attenuationFactor *spotIntensity);
	}
    finalColor = ambient + diffuse + specular;
    return finalColor * light.intensity;
}

float4 IterateLights(Material mat,uint startOffset, uint lightCount,float4 finalColor, float3 normalized, float3 worldPosition, float3 viewDirection)
{
	for (uint i = 0; i < lightCount; i++)
	{
		uint lightIndex = LightIndexList[startOffset + i];
		Light light = Lights[lightIndex];

		float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);

		switch (light.type)
		{
		case DIRECTIONAL_LIGHT:
		{
			result = CalculateDirectionalLight(light,mat, normalized, viewDirection);

		}
		break;
		case POINT_LIGHT:
		{
			result = CalculatePointLight(light,mat, normalized, worldPosition, viewDirection);
		}
		break;
		case SPOT_LIGHT:
		{
			result = CalculateSpotLight(light,mat, normalized, worldPosition, viewDirection);
		}
		break;
		}
		finalColor += result;
	}

	return finalColor;


}
