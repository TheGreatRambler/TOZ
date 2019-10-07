#pragma once

#include <cstdint>

#include "helpers.hpp"

public class Inputs {
	// Buttons
	private;
	bool A;
	bool B;
	bool X;
	bool Y;
	bool PLUS;
	bool MINUS;
	bool CAPTURE;
	bool HOME;
	bool DUP;
	bool DLEFT;
	bool DRIGHT;
	bool DDOWN;
	bool SL;
	bool SR;
	bool ZL;
	bool ZR;
	bool L;
	bool R;
	bool RSTICK;
	bool LSTICK;
	// Accelerometer and Gyro
	int16_t Accel_x;
	int16_t Accel_y;
	int16_t Accel_z;
	int16_t Gyro_1;
	int16_t Gyro_2;
	int16_t Gyro_3;
	// Timer to measure latency apparently
	// Incremented every frame
	uint8_t dataTimer = 0;
	// Stick data
	uint16_t leftStickX;
	uint16_t leftStickY;
	uint16_t rightStickX;
	uint16_t rightStickY;
	public;
	Inputs() {}
	void getData(char* dataToReturn) {
		// dataToReturn is the packet that will be sent
		// Standard full mode: sends all inputs and A and G data
		dataToReturn[0] = 0x30;
		// The timer
		dataToReturn[1] = (char) dataTimer;
		// The battely level and Connection info combined
		// Battery level is 8 (full)
		// Connection info is pro controller
		dataToReturn[2] = CreateByteWithNibbles(8, 0);
		// Button data
		char firstButtonData = 0;
		SetBit(firstButtonData, 0, Y);
		SetBit(firstButtonData, 1, X);
		SetBit(firstButtonData, 2, B);
		SetBit(firstButtonData, 3, A);
		SetBit(firstButtonData, 4, SR);
		SetBit(firstButtonData, 5, SL);
		SetBit(firstButtonData, 6, R);
		SetBit(firstButtonData, 7, ZR);
		dataToReturn[3] = firstButtonData;

		char secondButtonData = 0;
		SetBit(secondButtonData, 0, MINUS);
		SetBit(secondButtonData, 1, PLUS);
		SetBit(secondButtonData, 2, RSTICK);
		SetBit(secondButtonData, 3, LSTICK);
		SetBit(secondButtonData, 4, HOME);
		SetBit(secondButtonData, 5, CAPTURE);
		SetBit(secondButtonData, 6, 0); // Garbage bit
		SetBit(secondButtonData, 7, 0); // Charging grip (?)
		dataToReturn[4] = secondButtonData;

		char thirdButtonData = 0;
		SetBit(thirdButtonData, 0, DDOWN);
		SetBit(thirdButtonData, 1, DUP);
		SetBit(thirdButtonData, 2, DRIGHT);
		SetBit(thirdButtonData, 3, DLEFT);
		SetBit(thirdButtonData, 4, SR);
		SetBit(thirdButtonData, 5, SL);
		SetBit(thirdButtonData, 6, L);
		SetBit(thirdButtonData, 7, ZL);
		dataToReturn[5] = thirdButtonData;

		// Add stick data
		// Left
		uint32_t combinedStickDataLeft = leftStickY << 12 | leftStickX;
		unsigned char* combinedStickDataLeftValues = (unsigned char*) &combinedStickDataLeft;
		dataToReturn[6] = combinedStickDataLeftValues[1];
		dataToReturn[7] = combinedStickDataLeftValues[2];
		dataToReturn[8] = combinedStickDataLeftValues[3];
		// Right
		uint32_t combinedStickDataRight = rightStickY << 12 | rightStickX;
		unsigned char* combinedStickDataRightValues = (unsigned char*) &combinedStickDataRight;
		dataToReturn[9] = combinedStickDataRightValues[1];
		dataToReturn[10] = combinedStickDataRightValues[2];
		dataToReturn[11] = combinedStickDataRightValues[3];

		// Vibrator data (I don't know what the heck this does)
		// Just a hardcoded value
		dataToReturn[12] = 0x0A;

		// Increment timer
		if (dataTimer == 0xFF) {
			// Loop to beginning
			dataTimer = 0;
		} else {
			dataTimer++;
		}
	}
}
