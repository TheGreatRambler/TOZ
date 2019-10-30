#include <stdlib.h>
#include <libserialport.h>

#include "Serial_Raspi.hpp"

void prepare() {
	// serial console is already terminal-free
	// Make user aware
	puts("Mount serial device");
	// Load neccessary kernel modules
	system("modprobe dwc2");
	system("modprobe g_serial");
	// Open the port
	OpenPort();
	// Now ready for reading and writing
}
