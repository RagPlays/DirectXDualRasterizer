#ifndef MATHHELPERS_H
#define MATHHELPERS_H

#include <cmath>

namespace dae
{
	/* --- HELPER STRUCTS --- */
	struct Int2
	{
		int x{};
		int y{};
	};

	/* --- CONSTANTS --- */
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_DIV_2 = 1.57079632679489661923f;
	constexpr float PI_DIV_4 = 0.785398163397448309616f;
	constexpr float PI_2 = 6.283185307179586476925f;
	constexpr float PI_4 = 12.56637061435917295385f;

	constexpr float TO_DEGREES = (180.0f / PI);
	constexpr float TO_RADIANS(PI / 180.0f);

	/* --- HELPER FUNCTIONS --- */
	inline float Square(float a)
	{
		return a * a;
	}

	inline float Lerpf(float a, float b, float factor)
	{
		return ((1.f - factor) * a) + (factor * b);
	}

	inline bool AreEqual(float a, float b, float epsilon = FLT_EPSILON)
	{
		return std::abs(a - b) < epsilon;
	}

	inline int Clamp(const int v, int min, int max)
	{
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}

	inline float Clamp(float v, float min, float max)
	{
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}

	inline float Saturate(float v)
	{
		if (v < 0.f) return 0.f;
		if (v > 1.f) return 1.f;
		return v;
	}
}

#endif // !MATHHELPERS_H