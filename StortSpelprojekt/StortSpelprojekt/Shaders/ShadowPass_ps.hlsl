#include "CommonBuffers.hlsl"

SamplerState defaultSampleType : register(s0);

Texture2D screenTexture : register(t0);
Texture2D sceneDepth : register(t1);
Texture2D shadowDepth : register(t2);

static float2 poissonDiskSamples[16] =
{
    float2(0.7952771f, 0.8865569f),
	float2(0.05550706f, 0.4227468f),
	float2(0.6396513f, 0.4324232f),
	float2(-0.0771156f, 0.8869811f),
	float2(0.8800477f, 0.05700803f),
	float2(0.3543596f, -0.2346418f),
	float2(0.5773101f, -0.6936153f),
	float2(-0.1994669f, -0.3151286f),
	float2(-0.01460677f, -0.9079273f),
	float2(0.9680309f, -0.4074859f),
	float2(0.9791155f, -0.9828652f),
	float2(-0.6393556f, -0.8822003f),
	float2(-0.7635292f, -0.2459009f),
	float2(-0.9553224f, 0.6004817f),
	float2(-0.6042864f, 0.2452013f),
	float2(-0.719184f, 0.9991779f)
};

float3 WorldPosFromDepth(float depth, float2 uv)
{
    float z = depth * 2.0 - 1.0;

    float4 clipSpacePosition = float4(uv * 2.0 - 1.0, 0.0f, z);
    float4 viewSpacePosition = mul(invProjection, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w; // Perspective division
     
    float4 worldSpacePosition = mul(invView, viewSpacePosition);
    return worldSpacePosition.xyz;
}

int GetRandomIndex(float4 seed, int maxValue)
{
    float dot_product = dot(seed, float4(12.9898f, 78.233f, 45.164f, 94.673f));
    return int(float(maxValue) * frac(sin(dot_product) * 43758.5453f)) % maxValue;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    const float near = 0.01f; // NEAR PLANE
    const float far = 400.0f; // FAR PLANE
    float depth = sceneDepth.Sample(defaultSampleType, input.uv).x;
    float D = ((2.0f * near) / (far + near - depth * (far - near)));
        
    float4 position = float4(WorldPosFromDepth(D, input.uv), 1.0f);
    
    float3 dir = normalize(position.xyz - cameraPosition);
  //  return float4(dir, 1.0f);
    
    const int POSSION_DISK_SAMPLE_COUNT = 4;
    const float bias = 0.00001f;
    const float PCFSpread = 1.0f / 2028; //texel size av shadow mapp
    
    //float4 relativeViewPos = mul(shadowVP, position);
    float4 relativeViewPos = mul(shadowVP, position);

    float2 projectTexCoord;
    projectTexCoord.x = 0.5f + (relativeViewPos.x / relativeViewPos.w) / 2.0f;
    projectTexCoord.y = 0.5f - relativeViewPos.y / relativeViewPos.w / 2.0f;

   // return float4(projectTexCoord, 0.0f, 1.0f);
    
    float visibility = 1.0f;

	// kollar om den transformerade positionen är innanför sol kamerans vy
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		// positionens djup relativt till solens kamera
        float lightDepthValue = (relativeViewPos.z / relativeViewPos.w) - bias;

		//[unroll(POSSION_DISK_SAMPLE_COUNT)]
  //      for (int i = 0; i < POSSION_DISK_SAMPLE_COUNT; i++)
  //      {
  //          float4 seed = float4(input.uv.xyy, i);
  //          int index = GetRandomIndex(seed, 16);
            
  //          //float sampledDepthValue = shadowDepth.Sample(defaultSampleType, projectTexCoord + (poissonDiskSamples[index] * PCFSpread)).r;
  //          float sampledDepthValue = shadowDepth.Sample(defaultSampleType, projectTexCoord).r;

		//	// om positionens djup är större än skuggans djup som vi samplar så reducerar vi visibility variabeln
  //          if (lightDepthValue > sampledDepthValue)
  //          {
  //              visibility -= (1.0f / POSSION_DISK_SAMPLE_COUNT);
  //          }
  //      }
        
        //float sampledDepthValue = shadowDepth.SampleCmpLevelZero(defaultSampleType, projectTexCoord, lightDepthValue).r;
        float sampledDepthValue = shadowDepth.Sample(defaultSampleType, projectTexCoord).r;

			// om positionens djup är större än skuggans djup som vi samplar så reducerar vi visibility variabeln
        if (lightDepthValue > sampledDepthValue)
        {
            visibility = 0;
        }
 
    }
    

    return float4(D * visibility, D, visibility, 1.0f);
     
    //if (input.uv.x < 0.5f)
    //    return float4(0, 1, 0, 1);
    float4 diffuseColor = screenTexture.Sample(defaultSampleType, input.uv) * visibility;

    //const float gamma = 1.0f / 2.2f;
    //diffuseColor = pow(diffuseColor, gamma);
    
    diffuseColor.a = 1.0f;
    return diffuseColor;
}