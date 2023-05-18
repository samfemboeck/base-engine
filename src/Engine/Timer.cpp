#include "../pch.h"
#include "Timer.h"

ClockTimer::ClockTimer(long intervalMs) :
	intervalMs_(intervalMs)
{
	start_ = clock();
}

bool ClockTimer::update()
{
	auto now = clock();
	elapsedMs_ += (now - start_) / CLOCKS_PER_SEC * 1000;
	if (elapsedMs_ >= intervalMs_)
	{
		elapsedMs_ -= intervalMs_;
		start_ = now;
		return true;
	}

	return false;
}

TickCountTimer::TickCountTimer(long interval_ms) :
	intervalMs_(interval_ms),
	start_(std::chrono::high_resolution_clock::now())
{
}

bool TickCountTimer::update()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
	start_ = now;
	elapsedMs_ += diff;
	if (elapsedMs_ >= intervalMs_)
	{
		elapsedMs_ -= intervalMs_;
		return true;
	}

	return false;
}

long TickCountTimer::getElapsedMs()
{
	return elapsedMs_;
}

long TickCountTimer::getIntervalMs()
{
	return intervalMs_;
}

ScopeTimer::ScopeTimer(const std::string& methodName) :
	methodName_(methodName)
{
	start_ = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
	Data.push_back(std::format("{}: {} ms", methodName_, elapsed).c_str());
}