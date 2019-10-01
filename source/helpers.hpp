#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


char CreateByteWithNibbles(int hi, int lo) {
	char byte = 0;
	byte = (byte & 0xF0) | (lo & 0xF); // write low nibble
	byte = (byte & 0x0F) | ((hi & 0xF) << 4); // write high nibble
}

char SetBit(char charToModify, int index, bool value) {
	charToModify ^= (-(unsigned long) value ^ charToModify) & (1UL << index);
	return charToModify;
}

std::string GetStdoutFromCommand(std::string cmd) {

	std::string data;
	FILE* stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL)
				data.append(buffer);
		pclose(stream);
	}
	return data;
}

void WaitForDataToRead(int fd, unsigned char* data, int size) {
	bool finished = false;
	while (!finished) {
		int bytesRead = read(fd, data, size);
		if (bytesRead != 0) {
			// Actual data has been read
			finished = true;
		}
	}
}

void printCharArray(const char* buffer, int length) {
	// Print as hex
	for (int i = 0; i < length; i++) {
		std::cout << "0x"; // Make it pretty
		std::cout << std::hex << (int) buffer[i]; // Print hex
		std::cout << " "; // Just a space
	}
	std::cout << std::endl; // Print endline
}