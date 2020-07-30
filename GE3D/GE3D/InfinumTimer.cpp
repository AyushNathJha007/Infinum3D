#include "InfinumTimer.h"

using namespace std::chrono;
InfinumTimer::InfinumTimer()
{
	last = steady_clock::now();
}

float InfinumTimer::Mark() //Returns time elapsed since the last time Mark() was called
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float InfinumTimer::Peek() const //Returns the time elapsed since we last called Mark() without resetting Mark point
{
	return duration<float>(steady_clock::now() - last).count();
}