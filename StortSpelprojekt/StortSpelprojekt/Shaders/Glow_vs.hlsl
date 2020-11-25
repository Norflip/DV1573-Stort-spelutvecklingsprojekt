#include "CommonBuffers.hlsl"
#include "IO.hlsl"

VS_OUTPUT_GLOW main(VS_INPUT input)
{
    VS_OUTPUT_GLOW output;
    
    output.position = input.position;
    
    output.uv = input.uv;
    
    return output;
}