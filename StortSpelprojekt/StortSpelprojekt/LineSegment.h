#pragma once

struct LineSegment
{
	dx::XMFLOAT2 start;
	dx::XMFLOAT2 end;

	LineSegment(dx::XMFLOAT2 start, dx::XMFLOAT2 end) : start(start), end(end) {}

	inline float SqrLength()
	{
		float dx = (end.x - start.x);
		float dy = (end.y - start.y);
		return (dx * dx + dy * dy);
	}

	inline dx::XMFLOAT2 Direction()
	{
		float dx = (end.x - start.x);
		float dy = (end.y - start.y);
		float length = sqrtf(SqrLength());
		return dx::XMFLOAT2(dx / length, dy / length);
	}

	inline float GetDistance(const float& x, const float& y, float& t)
	{
		float edge1ToPointX = x - start.x;
		float edge1ToPointY = y - start.y;

		float edge1ToEdge2X = end.x - start.x;
		float edge1ToEdge2Y = end.y - start.y;

		float dot = edge1ToPointX * edge1ToEdge2X + edge1ToPointY * edge1ToEdge2Y;
		float lengthSqr = edge1ToEdge2X * edge1ToEdge2X + edge1ToEdge2Y * edge1ToEdge2Y;
		t = (lengthSqr != 0) ? dot / lengthSqr : -1;

		float xx, yy;
		if (t < 0)
		{
			xx = start.x;
			yy = start.y;
		}
		else if (t > 1)
		{
			xx = end.x;
			yy = end.y;
		}
		else {
			xx = start.x + t * edge1ToEdge2X;
			yy = end.y + t * edge1ToEdge2Y;
		}

		float dx = x - xx;
		float dy = y - yy;

		t = Math::Clamp(t, 0.0f, 1.0f);
		return sqrtf(dx * dx + dy * dy);
	}
};