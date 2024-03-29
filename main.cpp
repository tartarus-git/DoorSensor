#include <signal.h>
#include <wiringPi.h>

#include "Console.h"
#include "LifetimeLog.h"
#include "Camera.h"

#define SENSOR 15
#define SENSOR_SOURCE 27
#define BUZZER 16
#define BUTTON 1
#define BUTTON_SOURCE 13
#define GREEN_LED 4
#define RED_LED 5

#define STATE_SOFTENER_TRIGGER 20
#define SLEEP_TIME 10

#define CODE_WINDOW 10
#define CODE_COOLDOWN_TIME 200

#include "../rhythmCode.h"

bool isRunning = true;

int getRefSpan() {
	bool prevState = true;
	int span = 0;
	while (isRunning) {
		// Delaying before calculations because of continue optimization.
		// Also because it makes more sense in this situation.
		delay(SLEEP_TIME);
		span++;

		// If the cool down time has passed, start transferring control back to the control loop.
		if (span >= CODE_COOLDOWN_TIME) {
			Console::log("Reference span was too long.", 1);
			return 0;
		}

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevState) { continue; }

			// If everything is successful, return the reference span.
			return span;
		}

		// If button is off, set the previous state to off.
		prevState = false;
	}
	// This is only reached when the program is shutting down.
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

		// If cooldown time has passed, start transferring control back to the control loop.
		if (span >= CODE_COOLDOWN_TIME) {
			Console::log("A note wasn't within cooldown bounds. Invalid rhythm code.", 1);
			return false;
		}

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevState) { continue; }
			prevState = true;

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
			// If the note isn't within acceptable boundaries, delay for cooldown and start transferring control back to the control loop.
			Console::log("Invalid rhythm code. Cooling down...", 1);
			delay(SLEEP_TIME * CODE_COOLDOWN_TIME); // So that codes that are too long don't restart code recognition too fast.
			Console::log("Cooled down.", 1);
			return false;
		}
		// If the button is off, set the previous button state to off.
		prevState = false;
	}
	// This is only reached when the program is shutting down.
	return false;
}

bool validateRhythmCode() {
	// Use first span as reference span. This will be used in the calculations for the remaining rhythm.
	int refSpan = getRefSpan();
	// If the reference span is incorrect, report failure and transfer control back to the control loop.
	if (refSpan == 0) { return false; }
	Console::log("Reference span accepted.", 1);

	// If we've gotten this far, return the success/failure of the remaining code detection back to the control loop.
	return traceRhythm(refSpan);
}

void initPins() {
	pinMode(SENSOR_SOURCE, OUTPUT);
	pinMode(SENSOR, INPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(BUTTON, INPUT);
	pullUpDnControl(BUTTON, PUD_DOWN);
	pinMode(BUTTON_SOURCE, OUTPUT);
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

void signalHandler(int signal) {
	Console::log("Signal caught. Shutting down...", 2);
	isRunning = false;
}

int main() {
	// Initialize Console for log access.
	Console::init();
	if (Console::fIsOpen) { Console::log("Successfully opened log file."); }
	else { Console::log("Encountered error while opening log file."); }

	Console::log("Initializing signal handler...");

   	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGHUP, signalHandler);

	Console::log("Signal handler initialized. Starting lifetime logger...");

	// Periodically logs itself's existence to establish a map of power outages.
	if (LifetimeLog::start()) { Console::log("Successfully opened lifetime log file and started lifetime logger."); }
	else { Console::log("Encountered error while opening lifetime log file. Did not start lifetime logger."); }

	Console::log("Initializing door sensor...");

	wiringPiSetup();
	initPins();

	// Reset all devices.
	showDisarmed();
	digitalWrite(BUZZER, LOW);
	// Activate power source for the sensor.
	digitalWrite(SENSOR_SOURCE, HIGH);
	// Activate power source for the button.
	digitalWrite(BUTTON_SOURCE, HIGH);

	// Main control flags.
	bool armed = false;
	bool safe = true;
	bool state = true;

	// Helper flags.
	// stateSoftener helps us get past the oscillation of the sensor when the door is open.
	int stateSoftener = 0;
	bool prevButtonState = false;

	Console::log("Initialization complete. Initializing camera...");

	if (Camera::init()) { Console::log("Successfully initialized camera."); }
	else { Console::log("Encountered error while initializing camera."); }

	Console::log("Entering control loop...");

	do {
		// Delaying at the beginning of loop so that the following code can use continue.
		delay(SLEEP_TIME);

		// Check if the camera output file overflowed.
		if (Camera::overflowed) {
			Console::log("Camera output file overflowed. Recording has been disabled.", 2);
		}

		// Check if the door is open or not.
		if (digitalRead(SENSOR)) {
			if (!state) {
				if (stateSoftener == STATE_SOFTENER_TRIGGER) {
					Console::log("Door has been closed.", 1);
					state = true;
					stateSoftener = 0;
				} else { stateSoftener++; }
			}
		} else {
			if (state) {
				if (armed && safe) {
					Console::log("DOOR HAS BEEN OPENED WHILE ARMED. STARTING RECORDING AND SOUNDING ALARM...", 2);
					digitalWrite(BUZZER, HIGH);
					Camera::record(); // We should be using a thread pool here, how do you do that with std::thread? TODO
					safe = false;
				} else {
					Console::log("Door has been opened.", 1);
				}
				state = false;
			}
			stateSoftener = 0;
		}

		if (digitalRead(BUTTON)) {
			// Only activate if it's a key down event.
			if (prevButtonState) { continue; }
			prevButtonState = true;

			// If chip is armed, validate rhythm code before disarming.
			if (armed) {
				Console::log("Disarm attempted, validating rhythm code.", 1);
				if (validateRhythmCode()) {
					// Reset the buzzer.
					digitalWrite(BUZZER, LOW);

					// Disarm the chip.
					Console::log("Rhythm code is acceptable, chip has been disarmed.", 1);
					armed = false;
					showDisarmed();
					Camera::stop();
				}
				continue;
			}

			// If chip is disarmed, arm the chip.
			Console::log("Chip armed.", 1);
			showArmed();
			armed = true;
			safe = true;
			state = true;

			continue;
		}
		// If button wasn't pressed, set the previous state to off.
		prevButtonState = false;
	} while (isRunning);

	// Clean up and exit.
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(RED_LED, LOW);
	digitalWrite(BUZZER, LOW);
	digitalWrite(SENSOR_SOURCE, LOW);
	digitalWrite(BUTTON_SOURCE, LOW);
	// Dispose Console so log file gets closed.
	Console::dispose();
	// Dispose LifetimeLog so lifetime file gets closed. This blocks for a few seconds while waiting for the threads to join.
	LifetimeLog::stop();
	// Stop and dispose Camera.
	Camera::stop();
	Camera::dispose();

	Console::log("Shutdown complete.", 2);
	return 0;
}
