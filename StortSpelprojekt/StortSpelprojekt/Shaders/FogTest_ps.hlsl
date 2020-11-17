#include "CommonBuffers.hlsl"
#include "FractionalBrownianMotion.hlsl"

Texture2D screenTexture : register (t0);
Texture2D depthTexture : register (t1);

Texture2D day : register(t2);		
Texture2D dusk : register(t3);	
Texture2D night : register(t4);		
Texture2D endNight : register (t5);

SamplerState defaultSampleType : register (s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// this is supposed to get the world position from the depth buffer
float3 WorldPosFromDepth(float depth, float2 uv) {
    float z = depth;// *2.0 - 1.0;

    float4 clipSpacePosition = float4(uv * 2.0 - 1.0, 0.0f, z);
    float4 viewSpacePosition = mul(invProjection, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w; // Perspective division

    float4 worldSpacePosition = mul(invView, viewSpacePosition);
    return worldSpacePosition.xyz;
}

float4 main(PixelInputType input) : SV_TARGET
{
    float4 diff;
    float4 diff2;
    float4 final;
   
    float4 background = float4(0.0f,0.0f,0.0f,1.0f);
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    diffuseColor = screenTexture.Sample(defaultSampleType, input.uv);
    return diffuseColor;
    
    float depth = depthTexture.Sample(defaultSampleType, input.uv).x;

    // different id, different lerps between textures
    if (id == 0)
    {
        diff = day.Sample(defaultSampleType, depth);
        diff2 = dusk.Sample(defaultSampleType, depth);
        final = lerp(diff, diff2, factor);
    }
    if (id == 1)
    {
        diff = dusk.Sample(defaultSampleType, depth);
        diff2 = night.Sample(defaultSampleType, depth);
        final = lerp(diff, diff2, factor);
    }
    if (id == 2)
    {
        diff = night.Sample(defaultSampleType, depth);
        diff2 = endNight.Sample(defaultSampleType, depth);
        final = lerp(diff, diff2, factor);
    }
    if (id == 3) // this one keeps the last texture i place a while longer, wont be needed in the end product when we change id after we change chunk
    {
        diff = endNight.Sample(defaultSampleType, depth);
        diff2 = float4(diff.r, diff.g, diff.b, diff.a);
        final = lerp(diff, diff2, factor);
    }

    const float start = 30; // FOG START
    const float end = 60; // FOG END

    const float near = 0.01f; // NEAR PLANE
    const float far = 500.0f; // FAR PLANE

    float D = ((2.0f * near) / (far + near - depth * (far - near)));
    float fogFactor = saturate(((D * far) - start) / (end - start));
        
    float2 st = input.uv.xy;// / float2(1600, 800) * 3.;
    
    //st += st * abs(sin(time * 0.1)*3.0);

    float2 q = float2(0,0);
    q.x = fbm(st + 0.00 * time + (mousePos / 690));
    q.y = fbm(st + float2(1.0, 0.0) + (mousePos / 690));

    float2 r = float2(0, 0);
    r.x = fbm(st + 1.0 * q + float2(1.7, 9.2) + 0.15 * time +(mousePos / 690)); //2pi r
    r.y = fbm(st + 1.0 * q + float2(8.3, 2.8) + 0.126 * time + (mousePos / 690)); //2pi r

    float f = fbm(st + r + (mousePos / 690));
    float3 color = float3(0,0,0);

    //color = lerp(float3(0.101961, 0.919608, 0.666667),
    //    float3(0.666667, 0.666667, 0.498039),
    //    clamp((f * f) * 4.0, 0.0, 1.0));

    //color = lerp(color,
    //    float3(0, 0, 0.164706),
    //    clamp(length(q), 0.0, 1.0));

    //color = lerp(color,
    //    float3(0.666667, 1, 1),
    //    clamp(length(r.x), 0.0, 1.0));


    //return float4(I, I, I, 1.0f);
   // return diffuseColor;

    color += final; //Add ramp texture color to fog
   //float4 fogColor = float4(0.1f, 0.1f, 0.4f, 1.0f);
    float4 fogColor = float4(1, 1, 1, 1.0f);
    fogColor = float4(((f * f * f + .6 * f * f + .5 * f) * color).xyz, 1.0f);

    //fogColor = (f * f * f + .6 * f * f + .5 * f) * fogColor;
    //diffuseColor = diffuseColor + (background*0.3f);
    //fogColor = fogColor + (background * 0.5f); //Här färgas dimman om
    float4 result = lerp(diffuseColor, fogColor, fogFactor);
    return result;
    //return diffuseColor + float4(d, d, d, 1.0f);
}