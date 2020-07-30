#pragma once
#include<chrono>

class InfinumTimer
{
public:
	InfinumTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};
