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
float3 WorldPosFromDepth(float depth, float2 uv) 
{
    float4 clipSpacePosition = float4(uv * 2.0 - 1.0, 0.0f, depth);
    float4 viewSpacePosition = mul(invProjection, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w; // Perspective division

    float4 worldSpacePosition = mul(invView, viewSpacePosition);
    return worldSpacePosition.xyz;
}

float4 main(PixelInputType input) : SV_TARGET
{
    float someValue = 343; //1080/pi unsure on math here
    float4 diff;
    float4 diff2;
    float4 final;

    float4 background = float4(0.0f,0.0f,0.0f,1.0f);
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    diffuseColor = screenTexture.Sample(defaultSampleType, input.uv);

    float depth = depthTexture.Sample(defaultSampleType, input.uv).x;

    const float start = 1; // FOG START
    float end = 120;
    if(distanceToHouse == 0)
    { 
      end = 10000; // FOG END
    }

    const float near = 0.01f; // NEAR PLANE
    const float far = 500.0f; // FAR PLANE

    float D = ((2.0f * near) / (far + near - depth * (far - near)));
    float fogFactor = saturate(((D * far) - start) / (end - start));

    float2 st = input.uv.xy; 
    st.y = st.y * (9.f / 16.f);// divides uv with screen aspect ratio
    float2 q = float2(0,0);
    q.x = fbm(st + 0.00 * time + (mousePos / someValue));
    q.y = fbm(st + float2(1.0, 0.0) + (mousePos / someValue));

    float2 r = float2(0, 0);
    r.x = fbm(st + 1.0 * q + float2(1.7, 9.2) + 0.15 * time + (mousePos / (someValue)));
    r.y = fbm(st + 1.0 * q + float2(8.3, 2.8) + 0.126 * time + (mousePos / (someValue))); 

    float f = fbm(st + r + (mousePos / (someValue)));
    float3 color = float3(0,0,0);

    //float4 depthSample = depthTexture.Sample(defaultSampleType, input.uv);

    //depthSample.x = 1.f - depthSample.x;
    //depthSample.x = (far)*depthSample.x;
    //depthSample.x = abs(depthSample.x);
    //depthSample.x = pow(depthSample.x, 3.0f);

    //float depthValue = (1.f, clamp(depthSample.x, 0.f, 0.999f));

   // //different id, different lerps between textures
   //if (id == 0)
   //{
   //    diff = day.Sample(defaultSampleType, depthValue);
   //    diff2 = dusk.Sample(defaultSampleType, depthValue);
   //    final = lerp(diff, diff2, factor);
   //}
   //if (id == 1)
   //{
   //    diff = dusk.Sample(defaultSampleType, depthValue);
   //    diff2 = night.Sample(defaultSampleType, depthValue);
   //    final = lerp(diff, diff2, factor);
   //}
   //if (id == 2)
   //{
   //    diff = night.Sample(defaultSampleType, depthValue);
   //    diff2 = endNight.Sample(defaultSampleType, depthValue);
   //    final = lerp(diff, diff2, factor);
   //}
   //if (id == 3) // this one keeps the last texture i place a while longer, wont be needed in the end product when we change id after we change chunk
   //{
   //    diff = endNight.Sample(defaultSampleType, depthValue);
   //    diff2 = float4(diff.r, diff.g, diff.b, diff.a);
   //    final = lerp(diff, diff2, factor);
   //}

   //float4 colorSample = day.Sample(defaultSampleType, (1.f, clamp(depthSample.x, 0.f, 0.999f)));
   color += final; //Add ramp texture color to fog

   float4 fogColor = float4(1, 1, 1, 1.0f);

   //Math to multiply colors
   fogColor = float4((((pow(f,4) + .6 * pow(f,3) + .5 * f * f) * color) + pow(color, 3)).xyz, 1.0f);


   const float amount = 0.2f;  // higher = more intensity 
   const float power = (1 / distanceToHouse) * 100;   // higher = less space on screen
   const float4 vignetteColor = float4(.8f, .3f, 0, 3.f); 
   float dis = length(input.uv * 2 - 1);
   dis = dis / 1.41421;
   dis = pow(dis, power);
   
   float4 result = lerp(diffuseColor, fogColor, fogFactor);
   float4 vigcolor = float4(lerp(result, vignetteColor, 1.0f - pow(1 - dis * amount, 2)).rgb, 1.0f);



   return vigcolor;

}