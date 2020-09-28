struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	uint id	: SV_InstanceID;
};

struct VS_INPUT_SKELETON
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	uint3 boneID    : BONEID;
	float3 skinWeight: SKINWEIGHT;
	uint id	: SV_InstanceID;
};

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

struct VS_OUTPUT_SKY
{
	float4 position : SV_POSITION;
	float3 lPosition : LPOSITION;
	float2 uv : TEXCOORD0;
};

struct VS_OUTPUT_QUAD
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};