struct GlowVS_in
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
};

struct GlowVS_out
{
    float4 pos : SV_Position;
    float2 texcoord : TEXCOORD;
};

GlowVS_out GlowVS_main(GlowVS_in input)
{
    GlowVS_out output = (GlowVS_out) 0;
    
    output.pos = float4(input.pos, 1.0f);
    output.texcoord = input.texcoord;
    
    return output;
}