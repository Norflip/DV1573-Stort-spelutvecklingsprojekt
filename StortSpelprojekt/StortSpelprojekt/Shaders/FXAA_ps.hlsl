// Source for FXAA implementation on OpenGL
//http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html

Texture2D screenTexture : register (t0);
SamplerState defaultSampleType : register (s0);

static const float EDGE_THRESHOLD_MIN = 0.0312;
static const float EDGE_THRESHOLD_MAX = 0.125;

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

float RgbToLuma(float3 color)
{
	return sqrt(dot(color, float3(0.299, 0.587, 0.114)));
}

float4 main(VertexInputType input) : SV_TARGET
{
	float3 color = screenTexture.Sample(defaultSampleType, input.uv).rgb;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Luma at the current pixel
	float lumaCenter = RgbToLuma(color);

	// Luma at the four neighbours of the current pixel
	float lumaDown = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(0, -1)).rgb);
	float lumaUp = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(0, 1)).rgb);
	float lumaLeft = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(-1, 0)).rgb);
	float lumaRight = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(1, 0)).rgb);

	// Maximum and minimum luma around the current pixel
	float lumaMinimum = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
	float lumaMaximum = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

	// Luma delta
	float lumaRange = lumaMaximum - lumaMinimum;

	// If the luma variation is lower than the preset threshold, we dont calculate AA, or if the pixel is really dark
	if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMaximum * EDGE_THRESHOLD_MAX))
	{
		return float4(color, 1.0f);
	}

	// Next 4 neighbouring corners of the current pixel
	float lumaDownLeft = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(-1, -1)).rgb);
	float lumaUpRight = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(1, 1)).rgb);
	float lumaUpLeft = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(-1, 1)).rgb);
	float lumaDownRight = RgbToLuma(screenTexture.Sample(defaultSampleType, input.uv, float2(1, -1)).rgb);

	// Combine the four edges
	float lumaDownUp = lumaDown + lumaUp;
	float lumaLeftRight = lumaLeft + lumaRight;
	
	// Combine the corners
	float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
	float lumaDownCorners = lumaDownLeft + lumaDownRight;
	float lumaRightCorners = lumaDownRight + lumaUpRight;
	float lumaUpCorners = lumaUpRight + lumaUpLeft;

	// Calculate an estimation of the gradient along the horizontal and vertical axis
	float edgeHorizontal = abs(-2.0f * lumaLeft + lumaLeftCorners) + abs(-2.0f * lumaCenter + lumaDownUp) * 2.0f + abs(-2.0f * lumaRight + lumaRightCorners);
	float edgeVertical = abs(-2.0f * lumaUp + lumaUpCorners) + abs(-2.0f * lumaCenter + lumaLeftRight) * 2.0f + abs(-2.0f * lumaDown + lumaDownCorners);

	// Check if the local edge if horizontal or vertical
	bool isHorizontal = (edgeHorizontal >= edgeVertical);

	// Select the two neighboring texels lumas in the opposite direction to the local edge
	float luma1 = isHorizontal ? lumaDown : lumaLeft;
	float luma2 = isHorizontal ? lumaUp : lumaRight;

	// Calculate gradients in this direction
	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;

	// Which direction is the steepest
	bool isFirstSteepest = abs(gradient1) >= abs(gradient2);

	// Gradient in the corresponding direction
	float gradientScaled = 0.25 * max(abs(gradient1), abs(gradient2));

	//float stepLength = isHorizontal ? 
	return float4(color, 1.0f);
}