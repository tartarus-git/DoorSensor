#include "Console.h"

#include "SafePointer.h"
#include <cstring>
#include <stdio.h>
#include <ctime>
#include <fstream>

std::ofstream Console::f;
bool Console::fIsOpen;

void Console::init() {
	// Remove buffer so output gets written to the file as soon as possible.
	f.rdbuf()->pubsetbuf(0, 0);
	f.open("../Logs/log.txt", std::ios::app);
	if (f.is_open()) { fIsOpen = true; return; }
	fIsOpen = false;
}

void Console::dispose() { f.close(); }

char Console::timeMarker[TIME_MARKER_INIT_SIZE];

void Console::log(const char* message, int indent) {
	// Get the current time.
	const std::time_t currentTime = std::time(0);
	std::tm* localCurrentTime = std::localtime(&currentTime);
	// Convert to string and add indent if necessary.
	for (int i = 0; i < indent; i++) { timeMarker[i] = '\t'; }
	std::strftime(timeMarker + indent, TIME_MARKER_INIT_SIZE - indent, "[%c] ", localCurrentTime);
	// Get the length of the time marker up to the NUL character.
	size_t timeLength = strlen(timeMarker);

	// Get the length of the message.
	size_t length = strlen(message);

	// Create a SafePointer to a new buffer, which will be used to hold the edited message.
	// Make the length accomodate the time marker, the message, the newline and the NUL character.
	SafePointer<char*> buffer(new char[timeLength + length + 2], [](char* handle) { delete[] handle; });
	// I could of used unique_ptr for this, and maybe I should, I just forgot about it. This works just as well though doesn't it?

	// Copy time marker to buffer.
	std::memcpy(buffer.handle, timeMarker, timeLength);

	// Copy message to the buffer.
	char* messageStart = buffer.handle + timeLength;
	std::memcpy(messageStart, message, length);

	// Add the newline and the NUL characters.
	char* newline = messageStart + length;
	*newline = '\n';
	*(newline + 1) = '\0';

	// Write the buffer to the log file if it was successfully opened.
	if (fIsOpen) { f << buffer.handle; }
	// Print the buffer.
	printf(buffer.handle);

	// Clean up even though technically it isn't necessary because of SafePointer.
	buffer.dispose();
}

void Console::log(const char* message) { log(message, 0); }
