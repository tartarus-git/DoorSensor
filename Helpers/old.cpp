#include <stdio.h>
#include <wiringPi.h>

#define IN_PIN 15
#define OUT_PIN 16

int main() {
	printf("Running door sensor...\n");
	wiringPiSetup();

	pinMode(IN_PIN, INPUT);
	pinMode(OUT_PIN, OUTPUT);

	while (true) {
		if (digitalRead(IN_PIN)) {
			printf("Magnet detected.\n");
			digitalWrite(OUT_PIN, LOW);
		} else {
			printf("No magnet.\n");
			digitalWrite(OUT_PIN, HIGH);
		}
		delay(10);
	}

	return 0;
}
