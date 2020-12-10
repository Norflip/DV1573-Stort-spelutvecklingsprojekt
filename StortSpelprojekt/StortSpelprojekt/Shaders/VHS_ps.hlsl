#include "CommonBuffers.hlsl"

Texture2D screenTexture : register(t0);
Texture2D noiseTexture : register(t1);

SamplerState defaultSampleType : register(s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};



static const float _LinesDisplacement = 0.64f;
static const float _LinesSpeed = 1.0f;
static const float _LineAmount = 200f;

static const float _LensDistortion = 1.12f;


    
static const float _SineLinesAmount = 14.9f;
static const float _SineLinesSpeed = 20.0f;
static const float _SineLinesThreshold = 0.055f;
static const float _SineLinesDisplacement = 10.5f;
   

static const int _ColorBleedIterations = 10;
static const float _ColorBleedAmount = 1.15f;
static const float _ChromaticAberration = 4.32f;
static const float _Contrast = 0.924f;


//from https://www.shadertoy.com/view/ldjGzV
float2 screenDistort(float2 uv)
{
    uv -= 0.5;
    uv = uv * _LensDistortion * (1. / 1.2 + 2. * uv.x * uv.x * uv.y * uv.y);
    uv += .5;
    return uv;
}
 
float rand(in float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

static const float PI = 3.141592f;

float4 main(PixelInputType input) : SV_TARGET
{
    float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    diffuseColor = screenTexture.Sample(defaultSampleType, input.uv);

    // FROM UNITY 
    float time_x = time / 20.0f;
    float time_y = time;
    float time_z = time * 2.0f;
    float time_w = time * 3.0f;
    
    float2 uv = input.uv;
    
    //UV distortion
    uv = screenDistort(uv);
 
    float colR = 0;
    float colG = 0;
    float colB = 0;
    float offset = 0;
    
     //Getting solid lines
    float lines = step(0.5, frac(uv.y * _LineAmount + time_z * _LinesSpeed)) * 2.0 - 1.0;
    float linesDispl = lines * _LinesDisplacement;
 
    //Offsetting and wrapping the whole screen overtime
    uv.y = frac(uv.y + lerp(0.0, 0.4, frac(time_z * 2.0) * step(0.97, rand(floor(time_z * 2.0)))));
 
    //Constantly changing random noise values
    float random = rand(uv + time_x);
    
    //Sampling the noise texture while also making it move constantly
    float noise = noiseTexture.Sample(defaultSampleType, uv + rand(time_x)).x;
    
                //Getting random values from -1 to 1 every few frames to randomly change the speed and direction of the sine lines
    float sineLinesTime = time_y * _SineLinesSpeed * (rand(floor(time_y)) * 2.0 - 1.0);
    float sineLines = sin(uv.y * _SineLinesAmount * PI * 2.0 + sineLinesTime) * 0.5 + 0.5;
                //Lines with a random 0-1 value, to be used as mask
    float randLines = rand(round(uv.y * _SineLinesAmount + sineLinesTime));
    float sineLinesMask = step(randLines, _SineLinesThreshold);
    float sineLinesDispl = sineLines * sineLinesMask * _SineLinesDisplacement;
    

    //Multiple sampling for color bleeding
    for (int k = 0; k < _ColorBleedIterations; k++)
    {
        offset += lerp(0.8, _ColorBleedAmount, sin(time_y) * 0.5 + 0.5);
        
        colR += screenTexture.Sample(defaultSampleType, uv + float2(offset + _ChromaticAberration + linesDispl + sineLinesDispl, 0));
        colG += screenTexture.Sample(defaultSampleType, uv + float2(offset + _ChromaticAberration - linesDispl + sineLinesDispl, 0));
        colB += screenTexture.Sample(defaultSampleType, uv + float2(offset + linesDispl + sineLinesDispl, 0));
    }
    
    colR /= _ColorBleedIterations;
    colG /= _ColorBleedIterations;
    colB /= _ColorBleedIterations;
    float4 col = float4(colR, colG, colB, 1.0);
 
                //Reducing contrast
    col = lerp(0.5, col, _Contrast);
                //Grain noise
    col *= max(0.7, random);
                //Top and bottom noise
    col += smoothstep(abs(uv.y * 2.0 - 1.0) - 0.8, abs(uv.y * 2.0 - 1.0) - 0.99, noise);
                //Passing lines noise
    col += step(0.99, 1.0 - randLines) * step(sineLines, noise) * 0.2;
    return col;
    
    return diffuseColor;
}