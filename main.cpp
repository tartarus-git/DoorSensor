#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>

#include "Console.h"

#define SENSOR 15
#define SENSOR_SOURCE 27
#define BUZZER 16
#define BUTTON 1
#define GREEN_LED 4
#define RED_LED 5

#define SLEEP_TIME 10

#define CODE_LENGTH 5
#define CODE_WINDOW 10
#define CODE_COOLDOWN_TIME 200

// First note is omitted because it is always 1.
// Last note is omitted because the span it describes is useless.
const float code[CODE_LENGTH] = { 0.5f, 0.5f, 1, 2, 1 };

bool isRunning = true;

int getRefSpan() {
	bool prevState = true;
	int span = 0;
	while (isRunning) {
		// Delaying before calculations because of continue optimization.
		// Also because it makes more sense in this situation.
		delay(SLEEP_TIME);
		span++;

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevState) { continue; }
			prevState = true;

			// If the cool down time has passed, restart code recognition.
			if (span >= CODE_COOLDOWN_TIME) {
				span = 0;
				Console::log("Note length was too long, restarting code recognition...");
				continue;
			}

			// If everything is successful, return the reference span.
			return span;
		}
		// If button is off, set the previous state to off.
		prevState = false;
	}
	return 0;
}

bool traceRhythm(int refSpan) {
	bool prevState = true;
	int codeIndex = 0;
	int span = 0;
	while (isRunning) {
		// Sleep here because of continue optimization.
		// Also because it makes more sense.
		delay(SLEEP_TIME);
		span++;

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevState) { continue; }
			prevState = true;

			// If the cool down time has passed, restart code recognition.
			if (span >= CODE_COOLDOWN_TIME) { return false; }

			// Check whether note length is within acceptable boundaries.
			float difference = span - refSpan * code[codeIndex];
			if (difference <= CODE_WINDOW && difference >= -CODE_WINDOW) {
				// Return true if the entire code has successfully been entered.
				if (codeIndex == CODE_LENGTH - 1) { return true; }
				// Set up the values for the next entry.
				codeIndex++;
				span = 0;
				continue;
			}
			// If the note isn't within acceptable boundaries, restart code recognition.
			return false;
		}
		// If the button is off, set the previous button state to off.
		prevState = false;
	}
	return true;
}

void validateRhythmCode() {
	while (isRunning) {
		// Use first span as reference span. This will be used in the calculations for the remaining rhythm.
		int refSpan = getRefSpan();
		Console::log("Reference span accepted.");
		// If the remaining rhythm is correct, break out of the loop.
		if (traceRhythm(refSpan)) { break; }
		// If not, restart code recognition.
		Console::log("Invalid code. Restarting code recognition...");
	}
}

void initPins() {
	pinMode(SENSOR_SOURCE, OUTPUT);
	pinMode(SENSOR, INPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(BUTTON, INPUT);
	pullUpDnControl(BUTTON, PUD_DOWN);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
}

void showArmed() {
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(RED_LED, HIGH);
}

void showDisarmed() {
	digitalWrite(GREEN_LED, HIGH);
	digitalWrite(RED_LED, LOW);
}

void interruptHandler(int signal) {
	Console::log("Interrupt signal caught. Shutting down...");
	isRunning = false;
}

int main() {
	Console::log("Initializing interrupt handler...");

	struct sigaction sigIntHandler;

  	sigIntHandler.sa_handler = interruptHandler;
   	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;

   	sigaction(SIGINT, &sigIntHandler, nullptr);

	Console::log("Interrupt handler initialized. Initializing door sensor...");

	wiringPiSetup();
	initPins();

	// Reset all devices.
	showDisarmed();
	digitalWrite(BUZZER, LOW);
	// Activate power source for the sensor.
	digitalWrite(SENSOR_SOURCE, HIGH);

	// Main control flags.
	bool armed = false;
	bool safe = true;

	// Helper flag.
	bool prevButtonState = false;

	Console::log("Initialization complete. Entering control loop...");

	while (isRunning) {
		// Delaying at the beginning of loop so that the following code can use continue.
		delay(SLEEP_TIME);

		// Activate buzzer if door is opened.
		if (armed && safe && !digitalRead(SENSOR)) {
			digitalWrite(BUZZER, HIGH);
			safe = false;
			Console::log("DOOR HAS BEEN OPENED WHILE ARMED. SOUNDING ALARM.");
		}

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevButtonState) { continue; }
			prevButtonState = true;

			// If chip is armed, validate rhythm code before disarming.
			if (armed) {
				// Pause here until the correct code is entered.
				validateRhythmCode();
				// Reset the buzzer if needed.
				if (!safe) {
					digitalWrite(BUZZER, LOW);
					safe = true;
				}
				// Disarm the chip.
				showDisarmed();
				armed = false;
				Console::log("Rhythm code is acceptable, chip has been disarmed.");
				continue;
			}

			// If chip is disarmed, arm the chip.
			showArmed();
			armed = true;
			Console::log("Chip armed.");
			continue;
		}
		// If button wasn't pressed, set the previous state to off.
		prevButtonState = false;
	}

	// Clean up and exit.
	showDisarmed();
	digitalWrite(BUZZER, LOW);
	digitalWrite(SENSOR_SOURCE, LOW);
	return 0;
}
