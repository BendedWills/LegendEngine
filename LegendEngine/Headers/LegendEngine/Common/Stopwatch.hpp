#ifndef _LEGENDENGINE_STOPWATCH_HPP
#define _LEGENDENGINE_STOPWATCH_HPP

#include <chrono>

namespace LegendEngine
{
	class Stopwatch 
	{
	public:
		void Start();
		float GetElapsedMillis();
	private:
		std::chrono::high_resolution_clock::time_point startTime = 
			std::chrono::high_resolution_clock::now();
	};
}

#endif //_LEGENDENGINE_STOPWATCH_HPP
