#ifndef _LEGENDENGINE_STOPWATCH_HPP
#define _LEGENDENGINE_STOPWATCH_HPP

#include <chrono>

namespace LegendEngine
{
	class Stopwatch 
	{
	public:
		Stopwatch()
			:
			startTime(std::chrono::high_resolution_clock::now())
		{}

		/**
		 * @brief Sets the clock to the current time.
		 *  In other words, this function resets the timer.
		 */
		void Set();

		/**
		 * @returns The time (in milliseconds) since the last time Set was called.
		 */
		float GetElapsedMillis();
	private:
		std::chrono::high_resolution_clock::time_point startTime;
	};
}

#endif //_LEGENDENGINE_STOPWATCH_HPP
