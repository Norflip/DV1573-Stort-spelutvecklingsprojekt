#include "CommonBuffers.hlsl"
#include "IO.hlsl"



VS_OUTPUT main(VS_INSTANCE_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;
	output.worldPosition = mul(input.instanceWorld, input.position).xyz;
	
	output.position = mul(vp, float4(output.worldPosition,1));
	

	output.normal = normalize(mul(world, float4(input.normal,0)).xyz);
	output.tangent = normalize(mul(world, float4(input.tangent,0)).xyz);

	return output;
}