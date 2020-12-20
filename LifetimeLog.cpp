#include "LifetimeLog.h"

#include <fstream>
#include <thread>
#include <chrono>

#define TIME_STAMP_INIT_SIZE 26

std::ofstream LifetimeLog::f;

bool LifetimeLog::isAlive = true;
std::thread LifetimeLog::lifetimeThread;

bool LifetimeLog::start() {
	f.open("Logs/lifetime.txt");
	if (f.is_open()) {
		lifetimeThread = std::thread([]() {
			while (isAlive) {
				// Get the current time and output it to the lifetime log.
				std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
				f << std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count(); // I remember being able to cast instead of count().

				std::this_thread::sleep_for(std::chrono::seconds(5));
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
