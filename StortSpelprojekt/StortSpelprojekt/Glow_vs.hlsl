#include "CommonBuffers.hlsl"
#include "IO.hlsl"

VS_OUTPUT_GLOW GlowVS_main(VS_INPUT_GLOW input)
{
    VS_OUTPUT_GLOW output/* = (VS_OUTPUT_GLOW) 0*/;
    
    output.position = float4(input.position, 1.0f);
    output.uv = input.uv;
    
    return output;
}