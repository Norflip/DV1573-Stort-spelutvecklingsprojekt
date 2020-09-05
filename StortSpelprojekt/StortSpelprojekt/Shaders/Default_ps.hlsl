
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
	float d = dot(input.normal, float3(1,1,0));
	d = (d + 1) / 2.0f;

	return float4(d * input.uv.x, d * input.uv.y, 1.0f, 1.0f);
}
