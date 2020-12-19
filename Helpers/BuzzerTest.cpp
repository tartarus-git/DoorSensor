#include <wiringPi.h>

int main() {
	wiringPiSetup();
	pinMode(16, OUTPUT);
	while (true) {
		digitalWrite(16, HIGH);
		delay(500);
		digitalWrite(16, LOW);
		delay(500);
	}
	return 0;
}
