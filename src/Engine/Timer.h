#pragma once

#include <chrono>

class ClockTimer
{
public:
	ClockTimer(long intervalMs);
	bool update();
private:
	clock_t elapsedMs_ = 0;
	clock_t start_;
	long intervalMs_;
};

class TickCountTimer
{
public:
	TickCountTimer(long intervalMs);
	bool update();
	long getElapsedMs();
	long getIntervalMs();
private:
	long intervalMs_;
	long elapsedMs_ = 0;
	std::chrono::high_resolution_clock::time_point start_;
};

class ScopeTimer
{
public:
	inline static std::vector<std::string> Data;
	ScopeTimer(const std::string& methodName);
	~ScopeTimer();
	std::string methodName_;
	std::chrono::high_resolution_clock::time_point start_;
};
