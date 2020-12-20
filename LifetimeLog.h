#pragma once

#include <fstream>
#include <thread>

class LifetimeLog {
	static std::ofstream f;

	static bool isAlive;
	static std::thread lifetimeThread;
public:
	static bool start();
	static void stop();
};
