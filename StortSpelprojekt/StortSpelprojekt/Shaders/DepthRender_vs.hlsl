#include "CommonBuffers.hlsl"
#include "IO.hlsl"

struct D_VS_OUTPUT
{
    float4 position : SV_POSITION;
};

D_VS_OUTPUT main(VS_INPUT input)
{
    D_VS_OUTPUT output;
    output.position = mul(mvp, input.position);
  
	////output.position = input.position;

 //   if (input.weights.x > 0)
 //       output.position = mul(mul(input.position, boneTransforms[input.IDS.x]) * input.weights.x, mvp);

    return output;
}