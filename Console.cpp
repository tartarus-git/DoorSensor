#include "Console.h"

#include "SafePointer.h"
#include <cstring>
#include <stdio.h>
#include <ctime>

void Console::log(const char* message) {
	size_t length = strlen(message);

	// Get the current time.
	const std::time_t currentTime = std::time(0);
        const char* timeMarker = std::ctime(&currentTime);
	size_t timeLength = strlen(timeMarker);
	// This stuff is obsolete, use the new function instead.

	// Create a SafePointer to a new buffer, which will be used to hold the edited message.
	// Make the length accomodate the time marker, the message, the newline and the NUL character.
	SafePointer<char*> buffer(new char[1 + timeLength + 2 + length + 2], [](char* handle) { delete[] handle; });
	// I could of used unique_ptr for this, and maybe I should, I just forgot about it. This works just as well though doesn't it?

	// Copy time marker to buffer.
	*buffer.handle = '[';
	char* timeMarkerStart = buffer.handle + 1;
	std::memcpy(timeMarkerStart, timeMarker, timeLength);
	char* timeMarkerEnd = timeMarkerStart + timeLength;
	*timeMarkerEnd = ']';
	timeMarkerEnd += 1;
	*(timeMarkerEnd) = ' ';

	// Copy existing data to the buffer.
	char* messageStart = timeMarkerEnd + 1;
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
