Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState sampleType;
SamplerState sampleType2;

cbuffer DistortionBuffer
{
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortionScale;
	float distortionBias;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
};

float4 main (PixelInput input) : SV_TARGET
{	
	float4 noise1 = noiseTexture.Sample(sampleType, input.texCoord1);
	float4 noise2 = noiseTexture.Sample(sampleType, input.texCoord2);
	float4 noise3 = noiseTexture.Sample(sampleType, input.texCoord3);

	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	// Combine all three distorted noise results into a single noise result.
	float4 finalNoise = noise1 + noise2 + noise3;

	// Perturb the input texture Y coordinates by the distortion scale and bias values.  
	float perturbation = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	// Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
	float2 noiseCoords;
	noiseCoords.xy = (finalNoise.xy * perturbation) + input.tex.xy;
		
	float4 fireColor = fireTexture.Sample(sampleType2, noiseCoords.xy);
	float4 alphaColor = alphaTexture.Sample(sampleType2, noiseCoords.xy);

	// Set the alpha blending of the fire to the perturbed and distored alpha texture value.
	fireColor.a = alphaColor;

	return fireColor;
}