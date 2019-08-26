#pragma once
#include <chrono>
#include <iostream>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void Start()
	{
		m_StartPoint = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		m_EndPoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndPoint).time_since_epoch().count();

		auto duration = end - start;
		double ms = duration * 0.001;
		double sec = ms * 0.001;

		//std::cout << duration << "us (" << ms << "ms)\n" << std::endl;
		std::cout << duration << "us (" << sec << "s)\n" << std::endl;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartPoint;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_EndPoint;
};