#include "CommonBuffers.hlsl"
#include "IO.hlsl"



VS_OUTPUT main(VS_INSTANCE_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;
	output.worldPosition = mul(input.instanceWorld, input.position).xyz;
	
	output.position = mul(vp, float4(output.worldPosition,1));
	
	output.normal = normalize(mul(input.instanceWorld, input.normal).xyz);
	output.tangent = normalize(mul(input.instanceWorld, input.tangent).xyz);

	return output;
}