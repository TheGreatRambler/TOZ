#include <cstdio>
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>

#include "PacketCreator_Switch.hpp"

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
		}
		// Run this frame, whether we can or not
		inputProcess->run();
		// Increment current frame
		currentFrame++;
	}

	void closeFile() {
		currentStream.close();
	}
};