// Source for FXAA implementation on OpenGL and DirectX
// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf

#include "CommonBuffers.hlsl"

Texture2D screenTexture : register (t0);
SamplerState defaultSampleType : register (s0); // Try out anisotropic

static const float EDGE_THRESHOLD_MIN = 0.0312; // visible limit
static const float EDGE_THRESHOLD_MAX = 0.125; // high quality
static const float QUALITY[12] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.0f, 2.0f, 2.0f, 2.0f, 4.0f, 8.0f };

//static float index = 0;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float RgbToLuma(float3 color)
{
	return sqrt(dot(color, float3(0.299, 0.587, 0.114)));

	// or

	// return color.g * (0.587/0.299) + color.r;
}

//float Quality(int iteration)
//{
//	if (index >= 5)
//	{
//		int oldIndex = index;
//		index++;
//
//		return QUALITY[oldIndex];
//	}
//	else
//	{
//		return 1.0f;
//	}
//}

float4 main(PixelInputType input) : SV_TARGET
{
	float3 color = screenTexture.Sample(defaultSampleType, input.uv).rgb;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float2 inverseScreen = float2(1.0f / screenSize.x, 1.0f / screenSize.y);

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
		return float4(color, 1.0f);

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

	// Choose the step size (one pixel) according to the edge direction
	float stepLength = isHorizontal ? inverseScreen.x : inverseScreen.y;

	// Average luma in the correct direction
	float lumaLocalAverage = 0.0f;

	if (isFirstSteepest)
	{
		// Switch direction
		stepLength = -stepLength;
		lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
	}
	else
		lumaLocalAverage = 0.5f * (luma2 + lumaCenter);

	// Shift UV in the correct direction by half a pixel
	float2 currentUV = input.uv;

	if (isHorizontal)
		currentUV.y += stepLength * 0.5f;
	
	else
		currentUV.x += stepLength * 0.5f;

	// Compute offset for each iteration step in the right direction
	float2 offset = isHorizontal ? float2(inverseScreen.x, 0.0f) : float2(0.0f, inverseScreen.y);

	// Calculate UVS to explore on each side of the edge, orthogonally
	float2 uv1 = currentUV - offset;
	float2 uv2 = currentUV + offset;

	// Read the lumas at both current extremities of the exploration segment, and calculate the delta to the local average luma
	float lumaEnd1 = RgbToLuma(screenTexture.Sample(defaultSampleType, uv1).rgb);
	float lumaEnd2 = RgbToLuma(screenTexture.Sample(defaultSampleType, uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	// If the luma deltas are larger than the local gradient, we have reached the sides of the edge
	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;

	// If the side is not reached, we continue to explore this direction
	if (!reached1)
		uv1 -= offset;
	if (!reached2)
		uv2 += offset;
	
	// If both sides have not been reached, continue to explore
	if (!reachedBoth)
	{
		for (int i = 2; i < 12; i++)
		{
			// If needed, read luma in 1st direction, calculate delta
			if (!reached1)
			{
				lumaEnd1 = RgbToLuma(screenTexture.Sample(defaultSampleType, uv1).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}

			// If needed, read luma in opposite direction, calculate delta
			if (!reached2)
			{
				lumaEnd2 = RgbToLuma(screenTexture.Sample(defaultSampleType, uv2).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}

			// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge
			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;

			// If the side is not reached, we continue to explore
			// We use "Quality" to speedup the process
			if (!reached1)
				uv1 -= offset * QUALITY[i];
			if (!reached2)
				uv2 += offset * QUALITY[i];

			// If both sides have been reached, stop the exploration
			if (reachedBoth)
				break;

		}
	}

	// Calculate the distances to each extremity of the edge
	float distance1 = isHorizontal ? (input.uv.x - uv1.x) : (input.uv.y - uv1.y);
	float distance2 = isHorizontal ? (uv2.x - input.uv.x) : (uv2.y - input.uv.y);

	// In which direction is the extremity of the edge closer to
	bool isDirection1 = distance1 < distance2;
	float finalDistance = min(distance1, distance2);

	// Length of the edge
	float edgeThickness = (distance1 + distance2);

	// UV offset, read in the direction of the closest side
	float pixelOffset = -finalDistance / edgeThickness + 0.5f;

	// Is the luma at center smaller than the local average
	bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

	// If the luma at center is smaller than its neighbour, the delta luma at each end should be positive
	bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0f) != isLumaCenterSmaller;

	// If the luma variation is incorrect, do not offset
	float finalOffset = correctVariation ? pixelOffset : 0.0f;

	// Sub-pixel shifting
	// Full weighted average of the luma over the 3x3 neighbours
	float lumaAverage = (1.0f / 12.0f) * (2.0f * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

	// Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighbours
	float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0f, 1.0f);
	float subPixelOffset2 = (-2.0f * subPixelOffset1 + 3.0f) * subPixelOffset1 * subPixelOffset1;

	// Calculate a sub-pixel offset based on delta
	float subPixelFinalOffset = subPixelOffset2 * subPixelOffset2 * 0.75;

	// Pick the biggest of the two offsets
	finalOffset = max(finalOffset, subPixelFinalOffset);

	// Calculate final uv coordinates
	float2 finalUv = input.uv;
	if (isHorizontal)
		finalUv.y += finalOffset * stepLength;
	else
		finalUv.x += finalOffset * stepLength;

	// Read the colors at the new texture coordinates
	finalColor = screenTexture.Sample(defaultSampleType, finalUv);

	// Done
	return finalColor;
	//return float4(color, 1.0f);
	//return float4(0.0f, 1.0f, 0.0f, 1.0f);
}