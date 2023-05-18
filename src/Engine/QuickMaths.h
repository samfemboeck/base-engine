#pragma once

#include <glm/glm.hpp>
#include <string>

template<typename T>
struct Vec2
{	
	Vec2() = default;

	Vec2(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	Vec2(const glm::vec2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	operator const glm::vec2() const
	{
		return glm::vec2(x, y);
	}

	std::string str() const
	{
		return "{" + std::to_string(x) + ", " + std::to_string(y) + "}";
	}

	Vec2<T>& operator+=(const Vec2<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2<T> operator*=(float rhs)
	{
		return Vec2<T>(x * rhs, y * rhs);
	}

	Vec2<T> operator+(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x + rhs.x, y + rhs.y);
	}

	Vec2<T> operator*(float rhs) const
	{
		return Vec2<T>(x * rhs, y * rhs);
	}

	Vec2<T> operator/(Vec2<T> other)
	{
		return Vec2<T>(x / other.x, y / other.y);
	}

	Vec2<T> operator-(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x - rhs.x, y - rhs.y);
	}	
	
	Vec2<T> operator-(const float& f) const
	{
		return Vec2<T>(x - f, y - f);
	}

	Vec2<T> operator+(const float& f) const
	{
		return Vec2<T>(x + f, y + f);
	}

	T magnitude()
	{
		return sqrt(x * x + y * y);
	}

	T magnitude_squared()
	{
		return x * x + y * y;
	}

	Vec2<float> normalized()
	{
		float mag = magnitude();
		return Vec2<float>(x / mag, y / mag);
	}

	T dot(const Vec2<T>& other)
	{
		return x * other.x + y * other.y;
	}

	T cross(const Vec2<T>& other) const
	{
		return (x * other.y) - (y * other.x);
	}

	T x, y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

struct Random
{
	static float floatRange(float min, float max);
	static long longRange(long min, long max);
	static int intRange(int min, int max);
	static unsigned unsignedRange(unsigned min, unsigned max);
	static Vec2f vec2Range(const Vec2f& min, const Vec2f& max);
	static Vec2f unitVector();
	static Vec2f vec2FromAngle(float rad);
};