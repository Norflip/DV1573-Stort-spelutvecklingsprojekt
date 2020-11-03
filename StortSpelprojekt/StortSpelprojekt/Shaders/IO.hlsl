struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	uint id	: SV_InstanceID;
	uint i : SV_VertexID;
};

struct VS_INPUT_SKELETON
{
	float3 position : POSITION;
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

struct VS_INSTANCE_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
	
	uint id	: SV_InstanceID;
    row_major matrix instanceWorld : INSTANCEWORLD;
	float3 instancePos : INSTANCEPOS;
};

struct VS_OUTPUT_GRASS
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float tessFactor	 : TESSFACTOR;
};

struct HS_CONSTANT_DATA_OUTPUT_GRASS
{
	float edgeTesselation[2] : SV_TessFactor;
	uint   triangleIndex : TRIANGLE_INDEX;


};

struct HS_OUTPUT_GRASS
{
	float3 position	: POSITION;
	float2 tex		: TEXCOORD0;

};

struct DS_OUTPUT_GRASS
{
	float4 position	: SV_Position;
	centroid float2 tex	: TEXCOORD0;
	float3 normal   : NORMAL;
	float4 colour	: COLOUR;
	float  height : GRASS_HEIGHT;
	float  bladeHeight : BLADE_HEIGHT;
	float3 displacement : GRASS_DISPLACEMENT;
	float3 expandVector : ROTATION;
};


struct GS_OUTPUT_GRASS
{
	float4 position	: SV_Position;
	centroid float2 tex	: TEXCOORD0;
	float3 normal   : NORMAL;
	float4 colour	: COLOUR;
	float3 posToEye : EYE_VECTOR;
	float3 grassDirection : DIRECTION;
	float  fade : HAIRFADE;
};

