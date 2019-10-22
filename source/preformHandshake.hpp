#pragma once

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.hpp"

void startHandshake(int hidFileNum) {
	// This assumes that data has just been recieved, implying that the handshake has started
	unsigned char beginHandshake[12] = { 0x19, 0x81, 0x03, 0x07, 0x00, 0xA5, 0x02, 0x02, 0x7D, 0x00, 0x00, 0x64 };

  printf("Writing...\n");
	// Write data
	write(hidFileNum, beginHandshake, 12);
	printf("Reading...\n");

	// Wait for next data
	unsigned char next1[12];
	unsigned char dataToExpect1[12] = { 0x19, 0x01, 0x03, 0x07, 0x00, 0x91, 0x01, 0x00, 0x00, 0x00, 0x00, 0x24 };
	read(hidFileNum, next1, 12);

	if (memcmp(next1, dataToExpect1, 12) != 0) {
		// Wrong response
		printf("Wrong response! data: ");
		for(int i=0; i<12; ++i)
      std::cout << std::hex << (int)next1[i];
    printf("\n");
		return;
	}
	
	printf("Correct response!\n");

	// Get MAC address of wlan0 (main wifi)
	std::string macAdress = GetStdoutFromCommand("cat /sys/class/net/wlan0/address");
	unsigned int macBytes[6];
	// Scan hex codes into a mac address
	std::sscanf(macAdress.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &macBytes[0], &macBytes[1], &macBytes[2], &macBytes[3], &macBytes[4], &macBytes[5]);

	// Need to send MAC address
}