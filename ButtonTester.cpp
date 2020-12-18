#include <wiringPi.h>
#include <stdio.h>

int main() {
	wiringPiSetup();
	pinMode(1, INPUT);
	while (true) {
		if (digitalRead(1)) {
			printf("Pressed!\n");
		} else {
			printf("Open!\n");
		}
		delay(10);
	}
}
