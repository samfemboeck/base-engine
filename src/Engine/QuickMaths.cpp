#include "../pch.h"
#include "QuickMaths.h"

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

Vec2f Random::vec2Range(const Vec2f& min, const Vec2f& max)
{
	std::uniform_real_distribution<float> dX(min.x, max.x); // define the range
	std::uniform_real_distribution<float> dY(min.y, max.y); // define the range
	return { dX(gen), dY(gen) };
}

Vec2f Random::vec2FromAngle(float rad)
{
	return { sin(rad), cos(rad) };
}

unsigned Random::unsignedRange(unsigned min, unsigned max)
{
	std::uniform_int_distribution<unsigned> d(min, max);
	return d(gen);
}

float Random::floatRange(float min, float max)
{
	std::uniform_real_distribution<float> d(min, max); // define the range
	return d(gen);
}

long Random::longRange(long min, long max)
{
	std::uniform_int_distribution<long> d(min, max);
	return d(gen);
}

int Random::intRange(int min, int max)
{
	std::uniform_int_distribution d(min, max); // define the range
	return d(gen);
}

Vec2f Random::unitVector()
{
	std::uniform_real_distribution<float> d(-1.0f, 1.0f);
	Vec2f ret = { d(gen), d(gen) };
	return ret.normalized();
}