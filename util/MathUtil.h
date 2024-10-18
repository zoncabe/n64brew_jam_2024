#pragma once

#include <stddef.h>

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class MathUtil
{
public:
	static size_t ComputeNextPowerOfTwo(size_t next_power_of_two_alignment);

	static glm::vec3 TransformVector(const glm::vec3& direction, const glm::quat& orientation);

	static float DegreesToRadians(float degrees);

	static float RadiansToDegrees(float radians);

private:
	MathUtil() = delete;
	~MathUtil() = delete;
};