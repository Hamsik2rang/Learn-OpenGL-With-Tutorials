#pragma once
#include <random>

float getRandom(const float& min, const float& max)
{
	static std::random_device rd;
	static std::mt19937 msEngine(rd());

	std::uniform_real_distribution<float> ud(min, max);

	return ud(msEngine);
}