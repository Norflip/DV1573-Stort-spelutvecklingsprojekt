Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);

SamplerState defaultSampleType : register (s0);

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	textureColor = diffuseMap.Sample(defaultSampleType, input.uv);

	float d = dot(input.normal, float3(1,1,0));
	d = (d + 1) / 2.0f;
		
	return textureColor;
}
