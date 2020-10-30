#pragma once
namespace dx = DirectX;
#define HASH2D_F(X,Y) std::hash<float>()(X) * 100000 + std::hash<float>()(Y)
#define HASH2D_I(X,Y) std::hash<int>()(X) * 100000 + std::hash<int>()(Y)

namespace Math
{
	constexpr float ToRadians = 0.017453f;
	constexpr float ToDegree = 57.295779f;
	constexpr float PI = 3.14159265359f;

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
	inline T Move(T a, T b, T delta)
	{
		if (abs(b - a) <= delta)
			return b;

		int sign = (b - a) >= 0.0f ? 1.0f : -1.0f;
		return a + sign * delta;
	}

	inline dx::XMFLOAT2 Lerp(dx::XMFLOAT2 a, dx::XMFLOAT2 b, float t)
	{
		return dx::XMFLOAT2(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t));
	}

	inline dx::XMFLOAT2 InverseLerp(dx::XMFLOAT2 a, dx::XMFLOAT2 b, float t)
	{
		return dx::XMFLOAT2(InverseLerp(a.x, b.x, t), InverseLerp(a.y, b.y, t));
	}

	inline dx::XMFLOAT3 Lerp(dx::XMFLOAT3 a, dx::XMFLOAT3 b, float t)
	{
		return dx::XMFLOAT3(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t));
	}

	inline dx::XMFLOAT2 Move (dx::XMFLOAT2 a, dx::XMFLOAT2 b, float delta)
	{
		float x = b.x - a.x;
		float y = b.y - a.y;

		float distanceSqr = x * x + y * y;

		if (distanceSqr == 0 || (delta >= 0 && distanceSqr <= delta * delta))
			return b;

		float dist = sqrtf(distanceSqr);
		return dx::XMFLOAT2(a.x + x / dist * delta, a.y + y / dist * delta);
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

	inline std::vector<dx::XMFLOAT2> SmoothCurve(dx::XMFLOAT2 a, dx::XMFLOAT2 b, dx::XMFLOAT2 center, float smoothness)
	{
		const int pointsLength = 3;
		const int curvedLength = (pointsLength * (int)(roundf(smoothness))) - 1;

		std::vector<dx::XMFLOAT2> points;
		std::vector<dx::XMFLOAT2> curvedPoints(curvedLength);
		smoothness = std::max(smoothness, 1.0f);

		float t = 0.0f;

		for (int pointInTimeOnCurve = 0; pointInTimeOnCurve < curvedLength + 1; pointInTimeOnCurve++)
		{
			t = InverseLerp<float>(0, curvedLength, pointInTimeOnCurve);

			points.clear();
			points.resize(pointsLength);
			points.push_back(a);
			points.push_back(center);
			points.push_back(b);

			for (int j = pointsLength - 1; j > 0; j--)
			{
				for (int i = 0; i < j; i++)
				{
					float x = (1 - t) * points[i].x + t * points[i + 1].x;
					float y = (1 - t) * points[i].y + t * points[i + 1].y;

					points[i] = dx::XMFLOAT2(x,y);
				}
			}

			curvedPoints.push_back(points[0]);
		}

		return curvedPoints;
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
	inline float ShortestRotation(float currentDir, float nextDir)
	{
		float returnValue = 0;

		if (abs(nextDir - currentDir) < PI)
			returnValue = nextDir - currentDir;
		else if (currentDir < nextDir)
			returnValue = nextDir - currentDir - PI * 2.0f;
		else
			returnValue = nextDir - currentDir + PI * 2.0f;
		return returnValue;
	}
}