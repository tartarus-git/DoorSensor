#include <wiringPi.h>
#include <stdio.h>

int main() {
	wiringPiSetup();
	pinMode(1, INPUT);
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	bool prevState = false;
	while (true) {
		if (digitalRead(1)) {
			if (!prevState) {
				printf("Pressed!\n");
				prevState = true;
			}
		} else {
			if (prevState) {
				printf("Open!\n");
				prevState = false;
			}
		}
		delay(10);
	}
}
