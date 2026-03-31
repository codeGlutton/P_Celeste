#pragma once

class MathUtils
{
public:
	template<typename T>
	static T Lerp(const T& p0, const T& p1, float t)
	{
		return p0 + (p1 - p0) * t;
	}
	template<typename T>
	static T BezierQuad(const T& p0, const T& p1, const T& p2, float t)
	{
		float u = 1.0f - t;
		return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
	}
	template<typename T>
	static T BezierCubic(const T& p0, const T& p1, const T& p2, const T& p3, float t)
	{
		float u = 1.0f - t;
		return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;
	}
};
