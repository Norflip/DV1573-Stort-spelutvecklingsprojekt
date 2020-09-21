static const int POINT_LIGHT_COUNT = 16;
struct PointLight
{
    float4 lightColor;
    float3 lightPosition;
	float range;
    float3 attenuation;
};

cbuffer cb_Object : register(b0)
{
	row_major matrix mvp;
	row_major matrix world;
};

cbuffer cb_Material : register(b1)
{
	float4 matAmbient;
	float4 matDiffuse;
	float4 matSpecular;
	
	bool hasAlbedo;
	bool hasNormalMap;
}

cbuffer cb_Scene : register(b2)
{
    PointLight pointLights[POINT_LIGHT_COUNT];
    int nrOfPointLights;
    float3 sunDirection;
    float sunIntensity;
}