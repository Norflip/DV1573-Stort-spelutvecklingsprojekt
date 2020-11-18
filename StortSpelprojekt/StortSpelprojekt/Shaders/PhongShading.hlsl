#include "CommonBuffers.hlsl"

// Calculate light with the given pointlight
float4 CalculatePointLight(Light light, float3 normal, float3 objectPosition, float3 viewDirection)
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

	float diffuseFactor = dot(lightVec, normal);

	if (diffuseFactor > 0.0f)
	{
		float3 reflection = reflect(-lightVec, normal);
		float shine = pow(max(dot(reflection, viewDirection), 0.0f), 0.1f);

		diffuse = diffuseFactor * matDiffuse * light.lightColor;
		specular = shine * matSpecular * light.lightColor;
	}

	//float attenuationFactor = saturate(1.0f - distance / (pointLight.range * 5.0f));
	//attenuationFactor *= attenuationFactor;

	//float attenuationFactor = 1.0f / pointLight.attenuation.x + (pointLight.attenuation.y * distance) + (pointLight.attenuation.z * (distance * distance));

	float attenuationFactor = clamp(1.0f - distance * distance / (light.range * light.range), 0.0f, 1.0f);

	ambient = saturate(matAmbient * light.lightColor * attenuationFactor);
	diffuse = saturate(diffuse * attenuationFactor);
	specular = saturate(specular * attenuationFactor);
   
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
float4 CalculateDirectionalLight(Light light, float3 normal, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Can change from white to wanted color here
	float4 sunColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float3 lightDirection = normalize(light.lightDirection);

    float diffuseFactor = max(dot(normal, -lightDirection), 0.0f);

	if (diffuseFactor > 0.0f)
	{
        float3 reflection = reflect(lightDirection, normal);
		float spec = pow(max(dot(viewDirection, reflection), 0.0f), 1.0f);

		diffuse = diffuseFactor * sunColor * matDiffuse;
		specular = spec * sunColor * matSpecular;
	}

	diffuse = sunColor * matDiffuse;
	ambient = sunColor * matAmbient;

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

float4 CalculateSpotLight(Light light, float3 normal, float3 objectPosition, float3 viewDirection)
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

        diffuse = diffuseFactor * matDiffuse * light.lightColor * spotIntensity /* * light.intensity*/;
        specular = shine * matSpecular * light.lightColor * spotIntensity /* * light.intensity*/;
    }


    ambient = saturate(matAmbient * light.lightColor * attenuationFactor);
    diffuse = saturate(diffuse * attenuationFactor);
    specular = saturate(specular * attenuationFactor);
    //float4 temp = float4(1, 0, 0, 0);
    finalColor = ambient + diffuse + specular/*+temp*/;
    return finalColor * light.intensity;
}

