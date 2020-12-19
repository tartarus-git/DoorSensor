#include "LifetimeLog.h"

#include <fstream>
#include <thread>
#include <chrono>

std::ofstream LifetimeLog::f;

bool LifetimeLog::isAlive = true;

std::thread LifetimeLog::start() {
	f.open("Logs/lifetime.txt");
	if (f.is_open()) {
		std::thread lifetimeThread([]() {
			while (isAlive) {
				//char* time; // This whole file system thing needs to be done with a generalized version of Console.h.
				f << "I am alive.";
				std::this_thread::sleep_for(std::chrono::seconds(5)); // Check this line.
			}
		});
		return lifetimeThread;
	}
	return (std::thread)0;
}

void LifetimeLog::stop(std::thread lifetimeThread) {
	isAlive = false;
	lifetimeThread.join();
	f.close();
}
