struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

float4 main(VS_OUTPUT vs_out) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}