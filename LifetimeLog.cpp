#include "LifetimeLog.h"

#include <fstream>
#include <thread>
#include <ctime>
#include <chrono>

#define TIME_STAMP_INIT_SIZE 34

std::fstream LifetimeLog::f;

bool LifetimeLog::isAlive = true;
std::thread LifetimeLog::lifetimeThread;

bool LifetimeLog::start() {
	// TODO: Find out more about those flags that you put in here.
	f.open("Logs/lifetime.txt", std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);
	if (f.is_open()) {
		// Get the current time and output it to the lifetime log to announce startup.
		char timeStamp[TIME_STAMP_INIT_SIZE];
		// Reset only the values that need to be reset. The implied NUL from strftime is considered here as well.
		for (int i = 27; i < TIME_STAMP_INIT_SIZE; i++) { timeStamp[i] = 0; }
		const std::time_t currentTime = std::time(0);
        	std::strftime(timeStamp, TIME_STAMP_INIT_SIZE, "\n%c\n", std::localtime(&currentTime));
		f.write(timeStamp, TIME_STAMP_INIT_SIZE);
		// TODO: Consider popping timeStamp from the stack once this is over with. As long as it doesn't get optimized that is.
		// Ask on StackOverflow how to achieve this. You might have to use inline assembly language.
		lifetimeThread = std::thread([]() {
			unsigned long long counter;
			while (isAlive) {
				std::this_thread::sleep_for(std::chrono::seconds(5));

				// Increment lifetime counter to report uptime.
				f.seekg(-8, std::ios::end);
				f.read((char*)(&counter), 8);
				counter++;
				f.seekp(-8, std::ios::end);
				f.write((char*)(&counter), 8);
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
