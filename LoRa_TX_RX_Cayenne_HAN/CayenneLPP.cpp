#include "CayenneLPP.h"
CayenneLPP::CayenneLPP(uint8_t size) : maxsize(size) {
    buffer = (uint8_t*) malloc(size);
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
//    uint8_t[cursor] result;
//    memcpy(result, buffer, cursor);
//    return result;
    return buffer;
}

uint8_t CayenneLPP::copy(uint8_t* dst) {
    memcpy(dst, buffer, cursor);
    return cursor;
}

uint8_t CayenneLPP::addBit(uint8_t channel, uint8_t value, uint8_t bitSize) {
    
    buffer[cursor++] = bitSize; 
    buffer[cursor++] = channel;
    buffer[cursor++] = value >> 8; 

    return cursor;
}

uint8_t CayenneLPP::addByte(uint8_t channel, uint8_t value, uint8_t bitSize) {

    buffer[cursor++] = bitSize; 
    buffer[cursor++] = channel;
    buffer[cursor++] = value;

    return cursor;
}

uint8_t CayenneLPP::add2Bytes(uint8_t channel, uint16_t value, uint8_t bitSize) {

    buffer[cursor++] = bitSize; 
    buffer[cursor++] = channel;
    buffer[cursor++] = value >> 8;  // Store most significant byte
    buffer[cursor++] = value & 0xFF;  // Store least significant byte

    return cursor;
}
uint8_t CayenneLPP::add4Bytes(uint8_t channel, uint32_t value, uint8_t bitSize){
  
    buffer[cursor++] = bitSize; 
    buffer[cursor++] = channel;
    buffer[cursor++] = (value >> 24) & 0xFF;  // Store the most significant byte
    buffer[cursor++] = (value >> 16) & 0xFF;  // Store the second most significant byte
    buffer[cursor++] = (value >> 8) & 0xFF;   // Store the third most significant byte
    buffer[cursor++] = value & 0xFF;          // Store the least significant byte

    return cursor;
}

uint8_t CayenneLPP::addDigitalInput(uint8_t channel, uint8_t value) {
    if ((cursor + LPP_DIGITAL_INPUT_SIZE) > maxsize) {
        return 0;
    }
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_DIGITAL_INPUT; 
    buffer[cursor++] = value; 

    return cursor;
}

uint8_t CayenneLPP::addDigitalOutput(uint8_t channel, uint8_t value) {
    if ((cursor + LPP_DIGITAL_OUTPUT_SIZE) > maxsize) {
        return 0;
    }
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_DIGITAL_OUTPUT; 
    buffer[cursor++] = value; 

    return cursor;
}

uint8_t CayenneLPP::addAnalogInput(uint8_t channel, float value) {
    if ((cursor + LPP_ANALOG_INPUT_SIZE) > maxsize) {
        return 0;
    }
    
    int16_t val = value * 100;
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ANALOG_INPUT; 
    buffer[cursor++] = val >> 8; 
    buffer[cursor++] = val; 

    return cursor;
}

//when changing to 1 byte to much precision is gone
/*uint8_t CayenneLPP::addAnalogInput(uint8_t channel, float value) {
    if ((cursor + LPP_ANALOG_INPUT_SIZE) > maxsize) {
        return 0;
    }
    uint8_t scaledValue = static_cast<uint8_t>(value * 2);
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ANALOG_INPUT;
    buffer[cursor++] = scaledValue;
    return cursor;
}*/

/*uint8_t CayenneLPP::addAnalogOutput(uint8_t channel, float value) {
    if ((cursor + LPP_ANALOG_OUTPUT_SIZE) > maxsize) {
        return 0;
    }
    int16_t val = value * 100;
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ANALOG_OUTPUT;
    buffer[cursor++] = val >> 8; 
    buffer[cursor++] = val; 
    
    return cursor;
}*/

uint8_t CayenneLPP::addAnalogOutput(uint8_t channel, float value) {
    if ((cursor + LPP_ANALOG_OUTPUT_SIZE) > maxsize) {
        return 0;
    }
    uint8_t scaledValue = static_cast<uint8_t>(value * 2);
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ANALOG_OUTPUT;
    buffer[cursor++] = scaledValue;
    return cursor;
}

/*uint8_t CayenneLPP::addLuminosity(uint8_t channel, uint16_t lux) {
    if ((cursor + LPP_LUMINOSITY_SIZE) > maxsize) {
        return 0;
    }
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_LUMINOSITY; 
    buffer[cursor++] = lux >> 8; 
    buffer[cursor++] = lux; 

    return cursor;
}*/
uint8_t CayenneLPP::addLuminosity(uint8_t channel, uint16_t lux) {
    if ((cursor + LPP_LUMINOSITY_SIZE) > maxsize) {
        return 0;
    }
    uint8_t scaledLux = lux / 16;
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_LUMINOSITY;
    buffer[cursor++] = scaledLux;
    return cursor;
}

uint8_t CayenneLPP::addPresence(uint8_t channel, uint8_t value) {
    if ((cursor + LPP_PRESENCE_SIZE) > maxsize) {
        return 0;
    }
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_PRESENCE; 
    buffer[cursor++] = value; 

    return cursor;
}
// reduced one 1 byte when changing the int16_t to int8_t
/*uint8_t CayenneLPP::addTemperature(uint8_t channel, float celsius) {
    if ((cursor + LPP_TEMPERATURE_SIZE) > maxsize) {
        return 0;
    }
    int16_t val = celsius * 10;
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_TEMPERATURE; 
    buffer[cursor++] = val >> 8; 
    buffer[cursor++] = val; 

    return cursor;
}*/

uint8_t CayenneLPP::addTemperature(uint8_t channel, float celsius) {
    if ((cursor + LPP_TEMPERATURE_SIZE) > maxsize) {
        return 0;
    }
    int8_t val = celsius * 10;
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_TEMPERATURE; 
    buffer[cursor++] = val;
    return cursor;
}

uint8_t CayenneLPP::addRelativeHumidity(uint8_t channel, float rh) {
    if ((cursor + LPP_RELATIVE_HUMIDITY_SIZE) > maxsize) {
        return 0;
    }
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_RELATIVE_HUMIDITY; 
    buffer[cursor++] = rh * 2; 

    return cursor;
}
/*
uint8_t CayenneLPP::addRelativeHumidity(uint8_t channel, float rh) {
    if ((cursor + LPP_RELATIVE_HUMIDITY_SIZE) > maxsize) {
        return 0;
    }
    uint8_t rhScaled = static_cast<uint8_t>(rh * 2);
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_RELATIVE_HUMIDITY;
    buffer[cursor++] = rhScaled;
    return cursor;
}*/

uint8_t CayenneLPP::addAccelerometer(uint8_t channel, float x, float y, float z) {
    if ((cursor + LPP_ACCELEROMETER_SIZE) > maxsize) {
        return 0;
    }
    int16_t vx = x * 1000;
    int16_t vy = y * 1000;
    int16_t vz = z * 1000;
    
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ACCELEROMETER; 
    buffer[cursor++] = vx >> 8; 
    buffer[cursor++] = vx; 
    buffer[cursor++] = vy >> 8; 
    buffer[cursor++] = vy; 
    buffer[cursor++] = vz >> 8; 
    buffer[cursor++] = vz; 

    return cursor;
}
// when changing it to int8_t the precison is gone
/*uint8_t CayenneLPP::addAccelerometer(uint8_t channel, float x, float y, float z) {
    if ((cursor + LPP_ACCELEROMETER_SIZE) > maxsize) {
        return 0;
    }
    int8_t vx = static_cast<int8_t>(x * 100);
    int8_t vy = static_cast<int8_t>(y * 100);
    int8_t vz = static_cast<int8_t>(z * 100);
    
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_ACCELEROMETER; 
    buffer[cursor++] = vx; 
    buffer[cursor++] = vy; 
    buffer[cursor++] = vz; 

    return cursor;
}*/

uint8_t CayenneLPP::addBarometricPressure(uint8_t channel, float hpa) {
    if ((cursor + LPP_BAROMETRIC_PRESSURE_SIZE) > maxsize) {
        return 0;
    }
    int16_t val = hpa * 10;
    
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_BAROMETRIC_PRESSURE; 
    buffer[cursor++] = val >> 8; 
    buffer[cursor++] = val; 

    return cursor;
}

uint8_t CayenneLPP::addGyrometer(uint8_t channel, float x, float y, float z) {
    if ((cursor + LPP_GYROMETER_SIZE) > maxsize) {
        return 0;
    }
    int16_t vx = x * 100;
    int16_t vy = y * 100;
    int16_t vz = z * 100;
    
    buffer[cursor++] = channel; 
    buffer[cursor++] = LPP_GYROMETER; 
    buffer[cursor++] = vx >> 8; 
    buffer[cursor++] = vx; 
    buffer[cursor++] = vy >> 8; 
    buffer[cursor++] = vy; 
    buffer[cursor++] = vz >> 8; 
    buffer[cursor++] = vz; 

    return cursor;
}

uint8_t CayenneLPP::addGPS(uint8_t channel, float latitude, float longitude, float meters) {
    if ((cursor + LPP_GPS_SIZE) > maxsize) {
        return 0;
    }
    int32_t lat = latitude * 10000;
    int32_t lon = longitude * 10000;
    int32_t alt = meters * 100;
    
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
}
