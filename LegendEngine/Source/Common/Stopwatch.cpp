#include <LegendEngine/Common/Stopwatch.hpp>

using namespace LegendEngine;

void Stopwatch::Start()
{
	this->startTime = std::chrono::high_resolution_clock::now();
}

float Stopwatch::GetElapsedMillis()
{
	std::chrono::duration<float, std::milli> timeSpan = 
		std::chrono::high_resolution_clock::now() - startTime;
	return timeSpan.count();
}
