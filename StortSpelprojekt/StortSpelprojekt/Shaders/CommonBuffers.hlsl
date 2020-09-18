
cbuffer cb_Object : register(b0)
{
	matrix mvp;
	matrix world;
};

cbuffer cb_Scene : register(b1)
{

}

cbuffer cb_Material : register(b2)
{
	float4 matAmbient;
	float4 matDiffuse;
	float4 matSpecular;
	
	bool hasAlbedo;
	bool hasNormalMap;
}

