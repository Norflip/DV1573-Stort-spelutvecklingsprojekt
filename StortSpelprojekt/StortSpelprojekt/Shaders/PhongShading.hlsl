#include "CommonBuffers.hlsl"

float4 CalculatePointLight(PointLight pointLight, float3 normal, float3 objectPosition, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 light = pointLight.lightPosition - objectPosition;
	
	float distance = length(light);
	light = normalize(light);

	if (distance > pointLight.range)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float diffuseFactor = dot(light, normal);

	if (diffuseFactor > 0.0f)
	{
		float3 reflection = reflect(-light, normal);
		float shine = pow(max(dot(reflection, viewDirection), 0.0f), 10.0f); //matSpecular.w);

		diffuse = diffuseFactor * matDiffuse * pointLight.lightColor;
		specular = shine * matSpecular * pointLight.lightColor;
	}

	float attenuationFactor = 1.0f / pointLight.attenuation.x + (pointLight.attenuation.y * distance) + (pointLight.attenuation.z * (distance * distance));

	ambient = saturate(matAmbient * pointLight.lightColor * attenuationFactor);
	diffuse = saturate(diffuse * attenuationFactor);
	specular = saturate(specular * attenuationFactor);

	finalColor = ambient + diffuse + specular;

	return finalColor;
}

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
		float length1 = length(temp1); // sqrt(c1.x * c1.x + c1.y * c1.y + c1.z * c1.z);
		float length2 = length(temp2); // sqrt(c2.x * c2.x + c2.y * c2.y + c2.z * c2.z);

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

	normal = mul(normalmap, TBN);

	return normal;
}

float4 CalculateDirectionalLight(float3 lightDirection, float3 normal, float3 viewDirection)
{
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Can change from white to wanted color here
	float4 sunColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	lightDirection = normalize(lightDirection - float3(0.0f, 0.0f, 0.0f));

	float diffuseFactor = max(dot(normal, lightDirection), 0.0f);

	if (diffuseFactor > 0.0f)
	{
		float3 reflection = reflect(-lightDirection, normal);
		float spec = pow(max(dot(viewDirection, reflection), 0.0f), matSpecular.w);

		
		diffuse = diffuseFactor * sunColor * matDiffuse;
		specular = spec * sunColor * matSpecular;
	}

	ambient = sunColor * matAmbient;

	finalColor = ambient + diffuse + specular;

	return finalColor * sunIntensity;
}