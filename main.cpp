#include <stdio.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>

#define SENSOR
#define BUZZER
#define BUTTON
#define GREEN_LED
#define RED_LED

#define CODE_SLEEP_TIME 10

int code[7] = { 2, 1, 1, 2, 4, 2, 4 };

void testRhythmCode() {
	// Get reference span by listening to the first two taps.
	int span = 0;
	int refSpan;
	while (true) {
		if (digitalRead(BUTTON, HIGH)) {
			refSpan = span;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CODE_SLEEP_TIME));
	}

	int codeIndex = 0;
	int span = 0;
	while (true) {
		if (digitalRead(BUTTON, HIGH)) {
			
		}

		// Sleep for some time and increment the counter so that our loop has a sense of time.
		std::this_thread::sleep_for(std::chrono::milliseconds(CODE_SLEEP_TIME));
		span++;
	}
}

int main() {
	printf("Initializing door sensor...\n");
	wiringPiSetup();

	// Setup up pin modes.
	pinMode(SENSOR, INPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(BUTTON, INPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	printf("Door sensor is active.\n");

	bool armed = false;
	bool safe = true;

	while (true) {
		if (armed && safe) {
			if (!digitalRead(SENSOR)) {
				safe = false;
				digitalWrite(BUZZER, HIGH);
			}
		}
		if (digitalRead(BUTTON)) {
			if (armed) {
				// Do the rythmic code here.
				

				armed = false;
				if (!safe) { digitalWrite(BUZZER, LOW); safe = true; }
				continue;
			}
			armed = true;
		}
	}

	return 0;
}
