#include "CommonBuffers.hlsl"
#include "IO.hlsl"
#include "noiseSimplex.cginc"

float3 ApplyWind(float3 pos, float offset)
{
    const float maxOffset = 3.0f;   // max leaf offset distance
    
    const float windSpeed = 0.2f;
    const float windStrength = 0.6f;
    const float3 windDirection = normalize(float3(1, -0.1f, 1));
    
    const float noiseScale = 40.0f;
    
    float3 noiseSamplePoint = (pos / noiseScale) + time * windSpeed;
    float t = snoise(noiseSamplePoint) * saturate(offset / maxOffset);
    return pos + (windDirection * windStrength) * t;
}

VS_OUTPUT main(VS_INSTANCE_INPUT input)
{
    VS_OUTPUT output;
    output.uv = input.uv;
    
    output.worldPosition = mul(input.instanceWorld, input.position).xyz;
    output.worldPosition = ApplyWind(output.worldPosition, length(input.position.xyz - output.worldPosition));
    
    output.position = mul(vp, float4(output.worldPosition, 1));
	
    output.normal = normalize(mul(input.instanceWorld, float4(input.normal, 0))).xyz;
    output.tangent = normalize(mul(input.instanceWorld, float4(input.tangent, 0))).xyz;

    return output;
}