#include "CommonBuffers.hlsl"

Texture2D screenTexture : register (t0);
Texture2D depthTexture : register (t1);
SamplerState defaultSampleType : register (s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float random(float2 _st) {
    return frac(sin(dot(_st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(in float2 _st) {
    float2 i = floor(_st);
    float2 f = frac(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(a, b, u.x) +
        (c - a) * u.y * (1.0 - u.x) +
        (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5

float fbm(float2 _st) {

    float v = 0.0;
    float a = 0.5;
    float2 shift = float2(100.0, 0.0f);
    float2 vv = _st;
    
    // Rotate to reduce axial bias
    float2x2 rot = float2x2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));

    [unroll]
    for (int i = 0; i < NUM_OCTAVES; ++i) 
    {
        v += a * noise(vv);
        vv = mul((vv * 2.0), rot) + shift;
        a *= 0.5;
    }
    return v;
}

//float2 SphericalMapping (float2 uv)
//{
//
//    uv -= 0.5;
////    uv.x *= iResolution.x / iResolution.y;
//
//   // uv *= 2.2;
//    
//    //vec2 m = (iMouse.xy / iResolution.xy) - 0.5;
//    
//
//    uv = lerp(uv, normalize(uv) * (2.0 * asin(length(uv)) / PI), 0.5);
//    float3 n = float3(uv, sqrt(1.0 - uv.x * uv.x - uv.y * uv.y));
//    uv = normalize(uv) * (2.0 * asin(length(uv)) / PI);
//
//    uv *= 2.0;
//    return uv;
//}

float2 SphericalMapping(float3 direction)
{
    static const float PI = 3.14159265f;

    float r = length(direction);
    direction *= 1.f / r;
    float theta = acos(direction.z);
    float phi = atan2(direction.y, direction.x);
    //phi += (phi < 0) ? 2 * PI : 0; // only if you want [0,2pi)
    return float2(phi, theta);


    //float tu = asin(direction.x) / PI + 0.5f;
    //float tv = asin(direction.y) / PI + 0.5f;
    //return float2(tu, tv);
}


float4 main(PixelInputType input) : SV_TARGET
{
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

   // if (input.uv.x < 0.5f)
      //  return float4(0, 1, 0, 1);




    float depth = depthTexture.Sample(defaultSampleType, input.uv).x;
    //d = remap01(0.96f, 1.0f, d);
    // return float4(d, d, d, 1.0f);


    float start = 10;
    float end = 40;

    float near = 0.01f;
    float far = 500.0f;

    float D = ((2.0f * near) / (far + near - depth * (far - near)));
    float fogFactor = saturate(((D * far) - start) / (end - start));

    float distance = D * far;

    float3 direction = normalize(float3(0,0,1));

    // BROWNIAN

    float2 uv = input.uv * 2.0f - 1.0f;
    float3 cam_pos = float3(0, 0, -5);
    float3 rd = mul(float3(input.uv.x, -input.uv.y, 1.0f), mvp);


    float4 position = float4(input.uv.x, input.uv.y, 1.0f);
    float3 viewRay = mul(position, invProjection).xyz;

    return float4(viewRay, 1.0f);

    float2 st = input.uv.xy;// / float2(1600, 800) * 3.;
   // st = SphericalMapping(rd);

    //st += st * abs(sin(time * 0.1)*3.0);

    float2 q = float2(0,0);
    q.x = fbm(st + 0.00 * time);
    q.y = fbm(st + float2(1.0, 0.0));

    float2 r = float2(0, 0);
    r.x = fbm(st + 1.0 * q + float2(1.7, 9.2) + 0.15 * time);
    r.y = fbm(st + 1.0 * q + float2(8.3, 2.8) + 0.126 * time);

    float f = fbm(st + r);
    float3 color = float3(0,0,0);

    color = lerp(float3(0.101961, 0.619608, 0.666667),
        float3(0.666667, 0.666667, 0.498039),
        clamp((f * f) * 4.0, 0.0, 1.0));

    color = lerp(color,
        float3(0, 0, 0.164706),
        clamp(length(q), 0.0, 1.0));

    color = lerp(color,
        float3(0.666667, 1, 1),
        clamp(length(r.x), 0.0, 1.0));



    //return float4(I, I, I, 1.0f);
    diffuseColor = screenTexture.Sample(defaultSampleType, input.uv);

   // float4 fogColor = float4(0.1f, 0.1f, 0.4f, 1.0f);
    float4 fogColor = float4(1, 1, 1, 1.0f);
    fogColor = float4(((f * f * f + .6 * f * f + .5 * f) * color).xyz, 1.0f);

    //fogColor = (f * f * f + .6 * f * f + .5 * f) * fogColor;

    return lerp(diffuseColor, fogColor, fogFactor);
    //return diffuseColor + float4(d, d, d, 1.0f);
}