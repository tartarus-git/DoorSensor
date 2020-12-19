#include "LifetimeLog.h"

#include <fstream>
#include <thread>
#include <chrono>

std::ofstream LifetimeLog::f;

bool LifetimeLog::isAlive = true;
std::thread Console::lifetimeThread;

bool LifetimeLog::start() {
	f.open("Logs/lifetime.txt");
	if (f.is_open()) {
		lifetimeThread = std::thread([]() {
			while (isAlive) {
				//char* time; // This whole file system thing needs to be done with a generalized version of Console.h.
				f << "I am alive.";
				std::this_thread::sleep_for(std::chrono::seconds(5)); // Check this line.
			}
		});
		return true;
	}
	return false;
}

void LifetimeLog::stop() {
	isAlive = false;
	lifetimeThread.join();
	f.close();
}
