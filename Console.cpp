#include "Console.h"

#include "SafePointer.h"
#include <cstring>
#include <stdio.h>
#include <ctime>

void Console::log(const char* message) {
	// Get the current time.
	const std::time_t currentTime = std::time(0);
	tm* localCurrentTime = localtime(&currentTime);
	strftime(timeMarker, TIME_MARKER_INIT_SIZE, "[%c] ", localCurrentTime);
	// Get the length of the time marker up to the NUL character.
	size_t timeLength = strlen(timeMarker);

	// Create a SafePointer to a new buffer, which will be used to hold the edited message.
	// Make the length accomodate the time marker, the message, the newline and the NUL character.
	SafePointer<char*> buffer(new char[timeLength + length + 2], [](char* handle) { delete[] handle; });
	// I could of used unique_ptr for this, and maybe I should, I just forgot about it. This works just as well though doesn't it?

	// Copy time marker to buffer.
	std::memcpy(buffer.handle, timeMarker, timeLength);

	// Copy message to the buffer.
	char* messageStart = buffer.handle + timeLength;
	size_t length = strlen(message);
	std::memcpy(messageStart, message, length);

	// Add the newline and the NUL characters.
	char* newline = messageStart + length;
	*newline = '\n';
	*(newline + 1) = '\0';

	// Print the buffer.
	printf(buffer.handle);

	// Clean up even though technically it isn't necessary because of SafePointer.
	buffer.dispose();
}
