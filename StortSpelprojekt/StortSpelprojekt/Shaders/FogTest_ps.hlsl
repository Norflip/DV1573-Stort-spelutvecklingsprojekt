#include "CommonBuffers.hlsl"

Texture2D screenTexture : register (t0);
Texture2D depthTexture : register (t1);
Texture2D skyTexture : register (t5);
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

// this is supposed to get the world position from the depth buffer
float3 WorldPosFromDepth(float depth, float2 uv) {
    float z = depth;// *2.0 - 1.0;

    float4 clipSpacePosition = float4(uv * 2.0 - 1.0, 0.0f, z);
    float4 viewSpacePosition = mul(invProjection, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w; // Perspective division

    float4 worldSpacePosition = mul(invView, viewSpacePosition);
    return worldSpacePosition.xyz;
}

float raySphereIntersect(float3 r0, float3 rd, float3 s0, float sr) {
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    float a = dot(rd, rd);
    float3 s0_r0 = r0 - s0;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sr * sr);
    if (b * b - 4.0 * a * c < 0.0) {
        return -1.0;
    }
    return (-b - sqrt((b * b) - 4.0 * a * c)) / (2.0 * a);
}

float3 SphereNormal(float3 p, float3 center, float radius)
{
    float3 normal = float3((p.x - center.x) / radius, (p.y - center.x) / radius, (p.z - center.x) / radius);
    return normalize(normal);
}

float4 main(PixelInputType input) : SV_TARGET
{
   
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float depth = depthTexture.Sample(defaultSampleType, input.uv).x;

    const float start = 10; // FOG START
    const float end = 40; // FOG END

    const float near = 0.01f; // NEAR PLANE
    const float far = 500.0f; // FAR PLANE

    float D = ((2.0f * near) / (far + near - depth * (far - near)));
    float fogFactor = saturate(((D * far) - start) / (end - start));

//    
//    float3 position = mul(world, WorldPosFromDepth(D, input.uv));
//
//    float3 o = cameraPosition;
//    float3 d = normalize(position - cameraPosition);
//    float3 center = float3(0, 0, 0);
//    float radius = 10.0f;
//
//    float a = raySphereIntersect(o, d, center, radius);
//
//    float3 positionB = o + d * a;
//    
//    if (positionB.y < 5.0f)
//    {
//        return float4(0, 0, 0, 1);
//    }
//
//    float3 normal = SphereNormal(positionB, center, radius);
//    static const float PI = 3.14159265f;
//
//    float u = atan2(normal.x, normal.z) / (2 * PI) + 0.5;
//    float v = normal.y * 0.5 + 0.5;
//
//    // - r0: ray origin
//// - rd: normalized ray direction
//// - s0: sphere center
//// - sr: sphere radius
//
//
//
//
//    float2 post = SphericalMapping(d);
//    //return float4(post, 0.0f, 1.0f);
//    
    
    
    // BROWNIAN
    // BROWNIAN
    // BROWNIAN

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
   // return diffuseColor;
    float4 background = skyTexture.Sample(defaultSampleType, input.uv);

   // float4 fogColor = float4(0.1f, 0.1f, 0.4f, 1.0f);
    float4 fogColor = float4(1, 1, 1, 1.0f);
    fogColor = float4(((f * f * f + .6 * f * f + .5 * f) * color).xyz, 1.0f);

    //fogColor = (f * f * f + .6 * f * f + .5 * f) * fogColor;
    //diffuseColor = diffuseColor + (background*0.3f);
    float4 result = lerp(diffuseColor, fogColor, fogFactor);
    return result;
    //return diffuseColor + float4(d, d, d, 1.0f);
}