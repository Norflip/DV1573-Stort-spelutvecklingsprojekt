
cbuffer cb_Object : register(b0)
{
	row_major matrix mvp;
	row_major matrix world;
};

//cbuffer cb_Scene : register(b1)
//{
//
//}

cbuffer cb_Material : register(b1)
{
	float4 matAmbient;
	float4 matDiffuse;
	float4 matSpecular;
	
	bool hasAlbedo;
	bool hasNormalMap;
}


cbuffer cb_Lights : register(b2)
{
    float4 lightDirection;
    float4 lightColor;
    float3 lightPosition;
    float lightType;
    float3 attenuation;
    float intensity;
}
