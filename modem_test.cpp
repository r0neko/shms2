#include <string>
#include <iostream>

#include "HuaweiModem.h"

using namespace std;

HuaweiModem modem("/dev/ttyUSB2", "/dev/ttyUSB1");

int main() {
	cout << "core start\n";
	modem.voiceCall("REDACTEDOWNPHONENUMBER");
	modem.playFile("./messages/alert.wav");
	return 0;
}
