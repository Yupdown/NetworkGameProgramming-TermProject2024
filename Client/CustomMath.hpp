#pragma once

inline float bitwise_absf(float num)noexcept
{
	int& numAsInt = reinterpret_cast<int&>(num);
	numAsInt &= 0x7FFFFFFF;
	return num;
}

inline bool IsFloatZero(const float number)noexcept
{
	return bitwise_absf(number) <= FLT_EPSILON;
}

inline bool IsSameFloat(const float a_, const float b_)noexcept { return IsFloatZero(a_ - b_); }

inline bool IsZeroVector(const glm::vec3& _vec)noexcept
{
	return IsFloatZero(_vec.x) && IsFloatZero(_vec.y) && IsFloatZero(_vec.z);
}

inline bool IsZeroVector(const glm::vec2& _vec)noexcept
{
	return IsFloatZero(_vec.x) && IsFloatZero(_vec.y);
}

inline bool IsSameVector(const glm::vec3& _a, const glm::vec3& _b)noexcept
{
	return IsZeroVector(_a - _b);
}

inline bool IsSameVector(const glm::vec2& _a, const glm::vec2& _b)noexcept
{
	return IsZeroVector(_a - _b);
}

inline constexpr const float Deg2Rad(const float _degree)noexcept { return glm::radians(_degree); }

constexpr const glm::vec3 GetRightByQuat(const glm::quat& _quat) noexcept { return (glm::mat4_cast(glm::normalize(_quat)))[0]; }
constexpr const glm::vec3 GetUpByQuat(const glm::quat& _quat) noexcept { return (glm::mat4_cast(glm::normalize(_quat)))[1]; }
constexpr const glm::vec3 GetLookByQuat(const glm::quat& _quat) noexcept { return (glm::mat4_cast(glm::normalize(_quat)))[2]; }

constexpr const glm::vec3 bitwise_absv(const glm::vec3& _v)noexcept {
	return glm::vec3{
		bitwise_absf(_v.x),
		bitwise_absf(_v.y),
		bitwise_absf(_v.z)
	}; }