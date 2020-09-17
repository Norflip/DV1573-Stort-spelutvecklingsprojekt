
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

}

cbuffer SkeletonData : register(b3)
{
	float4x4 bones[60];
};