#include "CommonBuffers.hlsl"
#include "IO.hlsl"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;	
	output.worldPosition = mul(world, input.position).xyz;
	//output.worldPosition = mul(input.position, world).xyz;
	output.position = mul(mvp, input.position);
	//output.position = input.position * 0.5f;

//	output.position.x += input.id * 0.4f;
	//output.position.y += input.id * 0.4f;

	output.normal = normalize(mul(world, input.normal).xyz);
	output.tangent = normalize(mul(world, input.tangent).xyz);

	return output;
}