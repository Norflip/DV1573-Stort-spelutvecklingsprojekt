#include "CommonBuffers.hlsl"
#include "IO.hlsl"

//#define INSTANCED

#ifdef INSTANCED
#define INPUT VS_INSTANCE_INPUT
#else
#define INPUT VS_INPUT
#endif

Texture2D chunkData : register(t0);
Texture2D chunkNormalTexture : register(t1);
SamplerState m_samplerState : register(s0);

static const float TERRAIN_SCALE = 7.5f;
static const float TERRAIN_SIZE = 32;

float3 GetNormal(float2 uv, float texel, float height)
{
	//texel is one uint size, ie 1.0/texture size
    float t0 = chunkData.SampleLevel(m_samplerState, uv + float2(-texel, 0), 0).x * height;
    float t1 = chunkData.SampleLevel(m_samplerState, uv + float2(texel, 0), 0).x * height;
    float t2 = chunkData.SampleLevel(m_samplerState, uv + float2(0, -texel), 0).x * height;
    float t3 = chunkData.SampleLevel(m_samplerState, uv + float2(0, texel), 0).x * height;

    float3 va = normalize(float3(1.0f, 0.0f, t1 - t0));
    float3 vb = normalize(float3(0.0f, 1.0f, t3 - t2));
    return float3(cross(va, vb).rgb);
}

VS_OUTPUT main(INPUT input)
{
    VS_OUTPUT output;
    output.uv = input.uv;

	/*
		r channel: height [0-1]
		g channel: distance to road [0-1] 
	*/

    float4 position = input.position;
    float sampledHeight = chunkData.SampleLevel(m_samplerState, input.uv, 0).x;
    position.y += sampledHeight * TERRAIN_SCALE;

#ifdef INSTANCED
    output.worldPosition = mul(input.instanceWorld, position).xyz;
#else
	output.worldPosition = mul(world, position).xyz;
#endif
	
    output.position = mul(mvp, position);
	
    float3 normal = GetNormal(input.uv, 1.0f / (TERRAIN_SIZE + 1.0f), TERRAIN_SCALE);
    float3 bitangent = cross(normal, float3(0, 0, 1));
    float3 tangent = cross(normal, bitangent);
    
	// Changed to input.normal since it offers the most accurate normal for the terrain
	// The normals returned by the "GetNormal" function is all facing in the Z direction, which should be incorrect
#ifdef INSTANCED
    output.normal = normalize(mul(input.instanceWorld, float4(input.normal, 0.0f)).xyz);
    output.tangent = normalize(mul(input.instanceWorld, float4(input.tangent, 0.0f)).xyz);
#else
    output.normal = normalize(mul(world, float4(normal, 0)).xyz);
    output.tangent = normalize(mul(world, float4(input.tangent, 0)).xyz);
#endif

    return output;
}