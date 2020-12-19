#include <wiringPi.h>

#define OUT_PIN 16

int main() {
	wiringPiSetup();
	pinMode(16, OUTPUT);
	digitalWrite(16, LOW);
	return 0;
}
