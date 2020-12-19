#include <stdio.h>
#include <wiringPi.h>

int main() {
	wiringPiSetup();
	pinMode(15, INPUT);
	pinMode(27, OUTPUT);
	digitalWrite(27, HIGH);
	pullUpDnControl(15, PUD_DOWN);
	while (true) {
		if (digitalRead(15)) {
			printf("HIGH\n");
		} else {
			printf("LOW\n");
		}
		delay(10);
	}
	return 0;
}
