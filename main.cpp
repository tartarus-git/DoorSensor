#include <stdio.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>

#define SENSOR 15
#define BUZZER 16
#define BUTTON 1
#define GREEN_LED 4
#define RED_LED 5

#define CODE_SLEEP_TIME 10
#define CODE_LENGTH 6

float code[CODE_LENGTH] = { 1, 0.5f, 0.5f, 1, 2, 1 };
//float code[CODE_LENGTH] = { 1, 1, 1 };

bool testRhythmCode() {
failCheckpoint:
	printf("Checkpoint.\n");
	bool prevState = true;

	// Get reference span by listening to the first two taps.
	int span = 0;
	int refSpan;
	while (true) {
		if (digitalRead(BUTTON)) {
			if (prevState == false) {
				if (span >= 200) {
					goto failCheckpoint;
				}
				refSpan = span;
				prevState = true;
				break;
			}
		} else {
			prevState = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CODE_SLEEP_TIME));
		span++;
	}

	int codeIndex = 1;
	span = 0;
	while (true) {
		if (digitalRead(BUTTON)) {
			if (prevState == false) {
				if (span >= 200) {
					printf("Too long.\n");
					goto failCheckpoint;
				}
				float difference = span - refSpan * code[codeIndex];
				printf("%f\n", difference);
				if (difference <= 10 && difference >= -10) {
					if (codeIndex == CODE_LENGTH - 1) {
						return true;
					}
					codeIndex++;
				} else {
					goto failCheckpoint;
				}
				prevState = true;
				span = 0;
			}
		} else {
			prevState = false;
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
	pullUpDnControl(BUTTON, PUD_DOWN);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	printf("Door sensor is active.\n");

	digitalWrite(GREEN_LED, HIGH);
	digitalWrite(RED_LED, LOW);
	bool armed = false;
	bool safe = true;
	digitalWrite(BUZZER, LOW);

	int softener = 0;
	bool prevButtonState = false;
	while (true) {
		if (armed && safe) {
			if (!digitalRead(SENSOR)) {
				safe = false;
				digitalWrite(BUZZER, HIGH);
			}
		}
		if (digitalRead(BUTTON)) {
			if (!prevButtonState) {
				if (softener == 10) {
					softener = -1;
					printf("Button press detected.\n");
					if (armed) {
						if (testRhythmCode()) {
							armed = false;
							digitalWrite(GREEN_LED, HIGH);
							digitalWrite(RED_LED, LOW);
							if (!safe) { digitalWrite(BUZZER, LOW); safe = true; }
						}
					} else {
						digitalWrite(GREEN_LED, LOW);
						digitalWrite(RED_LED, HIGH);
						armed = true;
					}
					prevButtonState = true;
				}
				softener++;
			}
		} else {
			prevButtonState = false;
			softener = 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}
