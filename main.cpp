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
#define CODE_LENGTH 7

float code[CODE_LENGTH] = { 1, 0.5f, 0.5f, 1, 2, 1, 2 };

bool testRhythmCode() {
	// Get reference span by listening to the first two taps.
	int span = 0;
	int refSpan;
	while (true) {
		if (digitalRead(BUTTON, HIGH)) {
			refSpan = span;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CODE_SLEEP_TIME));
		span++;
	}

	int codeIndex = 1;
	span = 0;
	while (true) {
		if (digitalRead(BUTTON, HIGH)) {
			float difference = span - refSpan / code[codeIndex];
			if (difference <= 1 && difference >= -1) {
				if (codeIndex == CODE_LENGTH - 1) {
					return true;
				}
				codeIndex++;
				span = 0;
				continue;
			}
		}

		// Sleep for some time and increment the counter so that our loop has a sense of time.
		std::this_thread::sleep_for(std::chrono::milliseconds(CODE_SLEEP_TIME));
		span++;
	}

	return false;
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
				if (testRhythmCode()) {
					armed = false;
					digitalWrite(GREEN_LED, HIGH);
					digitalWrite(RED_LED, LOW);
					if (!safe) { digitalWrite(BUZZER, LOW); safe = true; }
				}
				continue;
			}
			digitalWrite(GREEN_LED, LOW);
			digitalWrite(RED_LED, HIGH);
			armed = true;
		}
	}

	return 0;
}
