#pragma once

#include <fstream>
#include <thread>

class LifetimeLog {
	static bool isAlive;
public:
	static std::ofstream f;

	static std::thread start();
	static void stop(std::thread lifetimeThread);
}
