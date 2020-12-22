#pragma once

#include <fstream>

// 28 + 10 <-- supports 11 levels of indentation.
#define TIME_MARKER_INIT_SIZE 38

class Console {
	static std::ofstream f;

	static char timeMarker[TIME_MARKER_INIT_SIZE];
public:
	static bool fIsOpen;

	Console() = delete;
	static void init();

	static void log(const char* message, int indent);
	static void log(const char* message);

	static void dispose();
};
