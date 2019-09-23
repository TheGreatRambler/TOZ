#pragma once

struct ProconPacket {
		uint8_t header[8];
		uint8_t unknown[5];
		uint8_t rightButtons;
		uint8_t middleButtons;
		uint8_t leftButtons;
		uint8_t sticks[6];
	};
