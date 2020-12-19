#include <wiringPi.h>

int main() {
	wiringPiSetup();
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
}
