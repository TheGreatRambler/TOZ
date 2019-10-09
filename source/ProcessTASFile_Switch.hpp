#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <string>
#include <vector>

#include "PacketCreator_Switch.hpp"
#include "helpers.hpp"

class TASProcess_Switch {
private:
	std::string currentFilename;
	std::ifstream currentStream;
	bool fileIsCurrentlyDone = false;
	bool isDone;
	InputStructure* inputs;
	Inputs* inputProcess;

	long currentFrame = 0;
	long currentProcessedFrame;

	bool frameHasBeenProcessed = false;

public:
	TASProcess_Switch() {
	}

	void setTASFile(std::string filename) {
		currentFilename = filename;
		currentStream = std::ifstream(currentFilename, std::ifstream::in);
		if (!currentStream.is_open()) {
			// Error opening file
			return;
		}
	}

	void setInputsInstance(InputStructure* currentInputs) {
		// Pointer to the Inputs object
		inputs = currentInputs;
	}

	void setInputProcessInstance(Inputs* currentInputs) {
		inputProcess = currentInputs;
	}

	void setCurrentFrame(long frame) {
		currentFrame = frame;
	}

	void setNextInputs(Inputs currentInputs) {
		// This function is run at 60 fps with the inputs object
		if (!frameHasBeenProcessed) {
			// Frame needs to be processed
			std::string currentLine;
			// It returns the stream object, so it needs to be converted to a boolean
			isDone = !!std::getline(currentStream, currentLine);
			if (currentStream.bad()) {
				// Some sort of error
				return;
			}
			// Have the line now
			// Just use a massive std::sscanf instance
			// Hopefully fits all the buttons
			char buttonPresses[64];
			// clang-format off
			 std::sscanf(currentLine.c_str(), "%ul %s %d;%d %d;%d %d;%d;%d %d;%d;%d",
				&currentProcessedFrame, buttonPresses,
				&(inputs->leftStickX), &(inputs->leftStickY), &(inputs->rightStickX), &(inputs->rightStickY),
				&(inputs->Accel_x), &(inputs->Accel_y), &(inputs->Accel_z),
				&(inputs->Gyro_1), &(inputs->Gyro_2), &(inputs->Gyro_3));
			// clang-format on
			// Time to process buttons
			std::vector<std::string> buttons = SplitString(std::string(buttonPresses), ";");
			clearEveryButton();
			for (std::string thisButton : buttons) {
				setButtonByName(thisButton, true);
			}
			// Frame has been processed
			frameHasBeenProcessed = true;
		}
		if (currentProcessedFrame == currentFrame) {
			// Frame needs to be run now
			inputProcess->setRunThisFrame(true);
			// Can now process next frame
			frameHasBeenProcessed = false;
		} else {
			// Tell input subsystem not to run this frame
			inputProcess->setRunThisFrame(false);
			// Make sure buttons are empty too
			clearEveryButton();
		}
		// Run this frame, whether we can or not
		inputProcess->run();
		// Increment current frame
		currentFrame++;
	}

	void clearEveryButton() {
		inputs->A = false;
		inputs->B = false;
		inputs->X = false;
		inputs->Y = false;
		inputs->LSTICK = false;
		inputs->RSTICK = false;
		inputs->L = false;
		inputs->R = false;
		inputs->ZL = false;
		inputs->ZR = false;
		inputs->PLUS = false;
		inputs->MINUS = false;
		inputs->DLEFT = false;
		inputs->DUP = false;
		inputs->DRIGHT = false;
		inputs->DDOWN = false;
	}

	void setButtonByName(std::string button, bool on) {
		// They go in order of most used to save processing power
		if (button == "KEY_A") {
			inputs->A = on;
		} else if (button == "KEY_B") {
			inputs->B = on;
		} else if (button == "KEY_X") {
			inputs->X = on;
		} else if (button == "KEY_Y") {
			inputs->Y = on;
		} else if (button == "KEY_L") {
			inputs->L = on;
		} else if (button == "KEY_R") {
			inputs->R = on;
		} else if (button == "KEY_ZL") {
			inputs->ZL = on;
		} else if (button == "KEY_ZR") {
			inputs->ZR = on;
		} else if (button == "KEY_PLUS") {
			inputs->PLUS = on;
		} else if (button == "KEY_MINUS") {
			inputs->MINUS = on;
		} else if (button == "KEY_DLEFT") {
			inputs->DLEFT = on;
		} else if (button == "KEY_DUP") {
			inputs->DUP = on;
		} else if (button == "KEY_DRIGHT") {
			inputs->DRIGHT = on;
		} else if (button == "KEY_DDOWN") {
			inputs->DDOWN = on;
		} else if (button == "KEY_LSTICK") {
			inputs->LSTICK = on;
		} else if (button == "KEY_RSTICK") {
			inputs->RSTICK = on;
		}
	}

	void closeFile() {
		currentStream.close();
	}
};