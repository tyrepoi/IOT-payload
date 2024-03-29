#include "CustomCayeneLPP.h"
CayenneLPP::CayenneLPP(uint8_t size)
: maxsize(size) {
	buffer = (uint8_t*)malloc(size);
	cursor = 0;
}

CayenneLPP::~CayenneLPP(void) {
	free(buffer);
}

void CayenneLPP::reset(void) {
	cursor = 0;
}

uint8_t CayenneLPP::getSize(void) {
	return cursor;
}

uint8_t* CayenneLPP::getBuffer(void) {
	return buffer;
}

uint8_t CayenneLPP::copy(uint8_t* dst) {
	memcpy(dst, buffer, cursor);
	return cursor;
}

uint8_t CayenneLPP::addBit(uint8_t channel, uint8_t type, uint8_t value) {
	buffer[cursor++] = channel;
	buffer[cursor++] = type;
	buffer[cursor++] = value >> 8;

	return cursor;
}

uint8_t CayenneLPP::addCustomByte(uint8_t channel, uint8_t type, float value, uint16_t resolution, uint8_t num_bytes) {
	buffer[cursor++] = channel;
	buffer[cursor++] = type;

	uint16_t combined_res_num = ((resolution << 3) | num_bytes);
	buffer[cursor++] = (combined_res_num >> 8) & 0xFF;  // Store the high byte
	buffer[cursor++] = combined_res_num & 0xFF;         // Store the low byte

	uint32_t valueScaled = value * resolution;

	for (int byteIndex = num_bytes - 1; byteIndex >= 0; --byteIndex) {
		// Shift the scaled value to the right to isolate the current byte
		uint8_t byteValue = (valueScaled >> (byteIndex * 8)) & 0xFF;
		// Store the extracted byte in the buffer
		buffer[cursor++] = byteValue;
	}
}

uint8_t CayenneLPP::addByte(uint8_t channel, uint8_t type, float value, uint8_t resolution) {
	if ((cursor + 3) > maxsize) {
		return 0;
	}
	uint8_t valueScaled = value * resolution;
	buffer[cursor++] = channel;
	buffer[cursor++] = type;
	buffer[cursor++] = valueScaled;

	return cursor;
}

uint8_t CayenneLPP::addWord(uint8_t channel, uint8_t type, float value, uint8_t resolution) {
	if ((cursor + 4) > maxsize) {
		return 0;
	}
	uint16_t valueScaled = value * resolution;
	buffer[cursor++] = channel;
	buffer[cursor++] = type;
	buffer[cursor++] = valueScaled >> 8;    // Store most significant byte
	buffer[cursor++] = valueScaled & 0xFF;  // Store least significant byte

	return cursor;
}

uint8_t CayenneLPP::addDoubleWord(uint8_t channel, uint8_t type, uint32_t value, uint8_t resolution) {
	if ((cursor + 6) > maxsize) {
		return 0;
	}
	uint32_t valueScaled = value * resolution;
	buffer[cursor++] = channel;
	buffer[cursor++] = type;
	buffer[cursor++] = (value >> 24) & 0xFF;  // Store the most significant byte
	buffer[cursor++] = (value >> 16) & 0xFF;  // Store the second most significant byte
	buffer[cursor++] = (value >> 8) & 0xFF;   // Store the third most significant byte
	buffer[cursor++] = value & 0xFF;          // Store the least significant byte

	return cursor;
}

uint8_t CayenneLPP::addFloat(uint8_t channel, uint8_t type, float value, uint8_t resolution) {
	int32_t scaledValue = value * 10000000;  // scale by 10^7 for 0.0000001 resolution

	buffer[cursor++] = channel;
	buffer[cursor++] = LPP_ADDFLOAT;  // Assuming LPP_ADDFLOAT is a defined constant for the data type

	// Add the individual bytes of the signed 32-bit integer to the buffer
	buffer[cursor++] = scaledValue >> 24;  // Most significant byte
	buffer[cursor++] = scaledValue >> 16;  // Second most significant byte
	buffer[cursor++] = scaledValue >> 8;   // Third most significant byte
	buffer[cursor++] = scaledValue;        // Least significant byte
	return cursor;
}

uint8_t CayenneLPP::add3Float(uint8_t channel, uint8_t type, float x, float y, float z, uint8_t resolution) {
	if (type == LPP_GPS_SIZE) {
		if ((cursor + 11) > maxsize) {
			return 0;
		}
		int32_t lat = x * 100 * resolution;  //resolution has to be 100
		int32_t lon = y * 100 * resolution;
		int32_t alt = z * 1;

		buffer[cursor++] = channel;
		buffer[cursor++] = LPP_GPS;
		buffer[cursor++] = lat >> 16;
		buffer[cursor++] = lat >> 8;
		buffer[cursor++] = lat;
		buffer[cursor++] = lon >> 16;
		buffer[cursor++] = lon >> 8;
		buffer[cursor++] = lon;
		buffer[cursor++] = alt >> 16;
		buffer[cursor++] = alt >> 8;
		buffer[cursor++] = alt;

		return cursor;
	} else {
		if ((cursor + 8) > maxsize) {
			return 0;
		}
		int16_t vx = x * resolution;  //addAccelerometer has to be 1000,
		int16_t vy = y * resolution;  //addGyrometer has to be 100
		int16_t vz = z * resolution;

		buffer[cursor++] = channel;
		buffer[cursor++] = type;
		buffer[cursor++] = vx >> 8;
		buffer[cursor++] = vx;
		buffer[cursor++] = vy >> 8;
		buffer[cursor++] = vy;
		buffer[cursor++] = vz >> 8;
		buffer[cursor++] = vz;

		return cursor;
	}
}