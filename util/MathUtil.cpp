// C++.
#include <math.h>
#define _USE_MATH_DEFINES

// Local.
#include "MathUtil.h"

size_t MathUtil::ComputeNextPowerOfTwo(size_t next_power_of_two_alignment)
{
	next_power_of_two_alignment--;
	next_power_of_two_alignment |= next_power_of_two_alignment >> 1;
	next_power_of_two_alignment |= next_power_of_two_alignment >> 2;
	next_power_of_two_alignment |= next_power_of_two_alignment >> 4;
	next_power_of_two_alignment |= next_power_of_two_alignment >> 8;
	next_power_of_two_alignment |= next_power_of_two_alignment >> 16;
	next_power_of_two_alignment++;
	return next_power_of_two_alignment;
}

glm::vec3 MathUtil::TransformVector(const glm::vec3& direction, const glm::quat& orientation)
{
	return orientation * direction;
}

float MathUtil::DegreesToRadians(float degrees)
{
	static const double DEG2RAD = (M_PI / 180.0f);
	return static_cast<float>(degrees * DEG2RAD);
}

float MathUtil::RadiansToDegrees(float radians)
{
	static const double RAD2DEG = (180.0f / M_PI);
	return static_cast<float>(radians * RAD2DEG);
}