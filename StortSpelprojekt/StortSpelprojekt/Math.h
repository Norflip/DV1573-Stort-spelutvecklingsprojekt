#pragma once
#include "DirectXHelpers.h"
#include <DirectXMath.h>
namespace dx = DirectX;

#define HASH2D_F(X,Y) std::hash<float>()(X) * 100000 + std::hash<float>()(Y)
#define HASH2D_I(X,Y) std::hash<int>()(X) * 100000 + std::hash<int>()(Y)

namespace Math
{
	constexpr float ToRadians = 0.017453f;
	constexpr float ToDegree = 57.295779f;

	template<typename T> 
	inline T InverseLerp(T a, T b, T t)
	{
		return (t - a) / (b - a);
	}

	template<typename T>
	inline T Lerp(T a, T b, T t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	inline T Remap(T imin, T imax, T omin, T omax, float v)
	{
		T t = InverseLerp(imin, imax, v);
		return Lerp(omin, omax, t);
	}

	inline float Pack3DVector(float x, float y, float z)
	{
		typedef unsigned char UC;
		UC cx = static_cast<UC>(((x + 1.0f) * 0.5f) * 255.0f);
		UC cy = static_cast<UC>(((y + 1.0f) * 0.5f) * 255.0f);
		UC cz = static_cast<UC>(((z + 1.0f) * 0.5f) * 255.0f);
		return static_cast<float>((cx << 16) | (cy << 8) | cz);
	}

	// DENNA SKA TILL HLSL
	inline void Unpack3DVector (float src, float& r, float& g, float& b)
	{
		// Unpack to the 0-255 range
		r = floor(src / 65536.0f);
		g = floor(fmod(src, 65536.0f) / 256.0f);
		b = fmod(src, 256.0f);

		//Unpack to the -1..1 range
		r = (r / 255.0f * 2.0f) - 1.0f;
		g = (g / 255.0f * 2.0f) - 1.0f;
		b = (b / 255.0f * 2.0f) - 1.0f;
	}

	inline float DistanceToLineSqr(float px, float py, float line0x, float line0y, float line1x, float line1y)
	{
		float dx = line1x - line0x;
		float dy = line1y - line0y;

		if ((dx == 0) && (dy == 0))
		{
			// It's a point not a line segment.
			dx = px - line0x;
			dy = py - line0y;
			return sqrtf(dx * dx + dy * dy);
		}
		else
		{
			// Calculate the t that minimizes the distance.
			float t = ((px - line0x) * dx + (py - line0y) * dy) / (dx * dx + dy * dy);

			// See if this represents one of the segment's
			// end points or a point in the middle.
			if (t < 0)
			{
				dx = px - line0x;
				dy = py - line0y;
			}
			else if (t > 1)
			{
				dx = px - line1x;
				dy = py - line1y;
			}
			else
			{
				float cx = line0x + t * dx;
				float cy = line0y + t * dx;

				dx = px - cx;
				dy = py - cy;
			}

			return sqrtf(dx * dx + dy * dy);
		}
	}
}