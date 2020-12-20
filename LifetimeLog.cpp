#include "LifetimeLog.h"

#include <fstream>
#include <thread>
#include <ctime>
#include <chrono>
#include <iostream>

#define TIME_STAMP_INIT_SIZE 40

std::fstream LifetimeLog::f;

bool LifetimeLog::isAlive = true;
std::thread LifetimeLog::lifetimeThread;

bool LifetimeLog::start() {
	f.open("Logs/lifetime.txt", std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);
	if (f.is_open()) {
		// Get the current time and output it to the lifetime log to announce startup.
		char timeStamp[TIME_STAMP_INIT_SIZE];
		for (int i = 0; i < TIME_STAMP_INIT_SIZE; i++) {
			timeStamp[i] = 0;
		}
		const std::time_t currentTime = std::time(0);
        	std::strftime(timeStamp, TIME_STAMP_INIT_SIZE, "\n%c\n", std::localtime(&currentTime));
		//f << timeStamp;
		f.write(timeStamp, TIME_STAMP_INIT_SIZE);
		// Consider popping timeStamp from the stack once this is over with. As long as it doesn't get optimized that is.
		std::cout << sizeof(unsigned long long) << std::endl;
		lifetimeThread = std::thread([]() {
			while (isAlive) {
				std::this_thread::sleep_for(std::chrono::seconds(5));

				// Increment lifetime counter to report uptime.
				f.seekg(-(sizeof(unsigned long long)), std::ios::end);
				unsigned long long counter = 0x4100000000000000;
				f.read((char*)(&counter), sizeof(unsigned long long));
				std::cout << std::hex << counter << std::endl;
				counter++;
				f.seekp((sizeof(unsigned long long)), std::ios::beg);
				//f << counter; // Will this actually do the thing I want or should I do this more explicitly with buffers and what not?
				f.write((char*)(&counter), sizeof(unsigned long long));
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
