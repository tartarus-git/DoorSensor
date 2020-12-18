#include <wiringPi.h>
#include <thread>
#include <chrono>

#define GREEN_LED 4
#define RED_LED 5

int main() {
	wiringPiSetup();
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	while (true) {
		digitalWrite(GREEN_LED, HIGH);
		digitalWrite(RED_LED, LOW);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		digitalWrite(GREEN_LED, LOW);
		digitalWrite(RED_LED, HIGH);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}
