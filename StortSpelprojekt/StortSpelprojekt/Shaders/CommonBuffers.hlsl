static const int POINT_LIGHT_COUNT = 16;
struct PointLight
{
    float4 lightColor;
    float3 lightPosition;
	float range;
    float3 attenuation;
	float paddingThree;
};

cbuffer cb_Object : register(b0)
{
	row_major matrix mvp;
	row_major matrix world;
	row_major matrix vp;
	row_major matrix wv;

};

cbuffer cb_Material : register(b1)
{
	float4 matAmbient;
	float4 matDiffuse;
	float4 matSpecular;
	
	bool hasAlbedo;
	bool hasNormalMap;
	float paddingOne;
	float paddingTwo;
}

cbuffer cb_Scene : register(b2)
{
	row_major matrix invProjection;
	row_major matrix invView;

	float3 cameraPosition;
	int id;
	float factor;
	float time;
	float2 pad2;
}

cbuffer cb_Lights : register(b3)
{
	PointLight pointLights[POINT_LIGHT_COUNT];
	int nrOfPointLights;
	float3 sunDirection;
	float sunIntensity;
	float3 pad3;
	
};

StructuredBuffer<float4x4> bones : register(t2);

struct Grass
{
	float4 position;
	float4 normal;
	float4 uv;
};

StructuredBuffer<Grass> grassStraws : register(t3);

StructuredBuffer<float4> grassIndices : register(t4);

StructuredBuffer<float4>   BCCoordinates : register(t5);

cbuffer cb_grass : register(b3)
{
	float pixelSize;
	float grassWidth;
	float grassRadius;
	float grassDisplacement;
};