#pragma once

#include <cstdio>
#include <ios>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>


char CreateByteWithNibbles(int hi, int lo) {
	char byte = 0;
	byte = (byte & 0xF0) | (lo & 0xF); // write low nibble
	byte = (byte & 0x0F) | ((hi & 0xF) << 4); // write high nibble
	return byte;
}

void SetBit(char& charToModify, int index, bool value) {
	// Modify value by reference
	if (value) {
		// Set bit
		charToModify |= 1UL << index;
	} else {
		// Clear bit
		charToModify &= ~(1UL << index);
	}
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

/*
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
*/

void printCharArray(const char* buffer, int length) {
	// Print as hex
	for (int i = 0; i < length; i++) {
		std::cout << "0x"; // Make it pretty
		std::cout << std::hex << (int) buffer[i]; // Print hex
		std::cout << " "; // Just a space
	}
	std::cout << std::endl; // Print endline
}

std::vector<std::string> SplitString(const std::string& str, const std::string& delim) {
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do {
		pos = str.find(delim, prev);
		if (pos == std::string::npos)
			pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty())
			tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}


char* GetExecutablePath() {
	char arg1[20];
	char exepath[PATH_MAX + 1] = { 0 };

	sprintf(arg1, "/proc/%d/exe", getpid());
	readlink(arg1, exepath, 1024);
	return exepath;
}
