#pragma once

#define TIME_MARKER_INIT_SIZE 512

class Console {
	static char timeMarker[TIME_MARKER_INIT_SIZE]; // Check if there is some sort of standard for the size of this (or calculate).
public:
	Console() = delete;

	static void log(const char* message);
};
