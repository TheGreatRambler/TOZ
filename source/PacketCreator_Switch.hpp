#pragma once

#include <cstdint>

#include "helpers.hpp"

struct InputStructure {
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
	// Stick data
	uint16_t leftStickX;
	uint16_t leftStickY;
	uint16_t rightStickX;
	uint16_t rightStickY;
};

class Inputs {
	// Buttons
private:
	// Timer to measure latency apparently
	// Incremented every frame
	uint8_t dataTimer = 0;
	// Determine wether it needs to run this frame
	bool runThisFrame = false;
	InputStructure iS;

public:
	Inputs() {
	}

	void setRunThisFrame(bool run) {
		runThisFrame = run;
	}

	InputStructure* getInputStruct() {
		return &iS;
	}

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
		SetBit(firstButtonData, 0, iS.Y);
		SetBit(firstButtonData, 1, iS.X);
		SetBit(firstButtonData, 2, iS.B);
		SetBit(firstButtonData, 3, iS.A);
		SetBit(firstButtonData, 4, iS.SR);
		SetBit(firstButtonData, 5, iS.SL);
		SetBit(firstButtonData, 6, iS.R);
		SetBit(firstButtonData, 7, iS.ZR);
		dataToReturn[3] = firstButtonData;

		char secondButtonData = 0;
		SetBit(secondButtonData, 0, iS.MINUS);
		SetBit(secondButtonData, 1, iS.PLUS);
		SetBit(secondButtonData, 2, iS.RSTICK);
		SetBit(secondButtonData, 3, iS.LSTICK);
		SetBit(secondButtonData, 4, iS.HOME);
		SetBit(secondButtonData, 5, iS.CAPTURE);
		SetBit(secondButtonData, 6, 0); // Garbage bit
		SetBit(secondButtonData, 7, 0); // Charging grip (?)
		dataToReturn[4] = secondButtonData;

		char thirdButtonData = 0;
		SetBit(thirdButtonData, 0, iS.DDOWN);
		SetBit(thirdButtonData, 1, iS.DUP);
		SetBit(thirdButtonData, 2, iS.DRIGHT);
		SetBit(thirdButtonData, 3, iS.DLEFT);
		SetBit(thirdButtonData, 4, iS.SR);
		SetBit(thirdButtonData, 5, iS.SL);
		SetBit(thirdButtonData, 6, iS.L);
		SetBit(thirdButtonData, 7, iS.ZL);
		dataToReturn[5] = thirdButtonData;

		// Add stick data
		// Left
		uint32_t combinedStickDataLeft = iS.leftStickY << 12 | iS.leftStickX;
		unsigned char* combinedStickDataLeftValues = (unsigned char*) &combinedStickDataLeft;
		dataToReturn[6] = combinedStickDataLeftValues[1];
		dataToReturn[7] = combinedStickDataLeftValues[2];
		dataToReturn[8] = combinedStickDataLeftValues[3];
		// Right
		uint32_t combinedStickDataRight = iS.rightStickY << 12 | iS.rightStickX;
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

	void run() {
		if (runThisFrame) {
			// Set false for later
			runThisFrame = false;
		}
	}
};