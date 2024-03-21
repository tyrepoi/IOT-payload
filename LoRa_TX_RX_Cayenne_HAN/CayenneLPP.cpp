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

uint8_t CayenneLPP::addBit(uint8_t channel, uint8_t value) {
    
    buffer[cursor++] = channel;
    buffer[cursor++] = LPP_ADDBIT;
    buffer[cursor++] = value >> 8; 

    return cursor;
}

uint8_t CayenneLPP::addCustomByte(uint8_t channel, uint16_t type, float value, uint16_t resolution, uint8_t num_bytes){
    buffer[cursor++] = channel;
    buffer[cursor++] = type;
    buffer[cursor++] = num_bytes;
    buffer[cursor++] = resolution;

    uint32_t valueScaled = value * resolution;

    for (int byteIndex = num_bytes - 1; byteIndex >= 0; --byteIndex) {
    // Shift the scaled value to the right to isolate the current byte
    uint8_t byteValue = (valueScaled >> (byteIndex * 8)) & 0xFF;
    // Store the extracted byte in the buffer
    buffer[cursor++] = byteValue;
    }

    return cursor;
}



uint8_t CayenneLPP::addByte(uint8_t channel, uint16_t type, uint8_t value, uint16_t resolution) {

    buffer[cursor++] = channel;
    buffer[cursor++] = LPP_ADDBYTE;
    buffer[cursor++] = value;

    return cursor;
}

uint8_t CayenneLPP::add2Bytes(uint8_t channel, uint16_t type, uint16_t value, uint16_t resolution)  {

    uint16_t valueScaled = value *resolution;
    buffer[cursor++] = channel;
    buffer[cursor++] = type;
    buffer[cursor++] = valueScaled >> 8;  // Store most significant byte
    buffer[cursor++] = valueScaled & 0xFF;  // Store least significant byte

    return cursor;
}

uint8_t CayenneLPP::add4Bytes(uint8_t channel, uint16_t type, uint32_t value, uint16_t resolution){
 
    buffer[cursor++] = channel;
    buffer[cursor++] = LPP_ADD4BYTES;
    buffer[cursor++] = (value >> 24) & 0xFF;  // Store the most significant byte
    buffer[cursor++] = (value >> 16) & 0xFF;  // Store the second most significant byte
    buffer[cursor++] = (value >> 8) & 0xFF;   // Store the third most significant byte
    buffer[cursor++] = value & 0xFF;          // Store the least significant byte

    return cursor;
}

uint8_t CayenneLPP::addFloat(uint8_t channel, uint16_t type, float value, uint16_t resolution){
    int32_t scaledValue = value * 10000000; // scale by 10^7 for 0.0000001 resolution

    buffer[cursor++] = channel;
    buffer[cursor++] = LPP_ADDFLOAT; // Assuming LPP_ADDFLOAT is a defined constant for the data type

    // Add the individual bytes of the signed 32-bit integer to the buffer
    buffer[cursor++] = scaledValue >> 24;  // Most significant byte
    buffer[cursor++] = scaledValue >> 16;  // Second most significant byte
    buffer[cursor++] = scaledValue >> 8;   // Third most significant byte
    buffer[cursor++] = scaledValue;        // Least significant byte
    return cursor;
}

uint8_t CayenneLPP::addDigitalInput(uint8_t channel, uint8_t value)
{
  if ((cursor + LPP_DIGITAL_INPUT_SIZE) > maxsize)
  {
    return 0;
  }
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_DIGITAL_INPUT;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addDigitalOutput(uint8_t channel, uint8_t value)
{
  if ((cursor + LPP_DIGITAL_OUTPUT_SIZE) > maxsize)
  {
    return 0;
  }
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_DIGITAL_OUTPUT;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addAnalogInput(uint8_t channel, float value)
{
  if ((cursor + LPP_ANALOG_INPUT_SIZE) > maxsize)
  {
    return 0;
  }

  int16_t val = value * 100;
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_ANALOG_INPUT;
  buffer[cursor++] = val >> 8;
  buffer[cursor++] = val;

  return cursor;
}

uint8_t CayenneLPP::addAnalogOutput(uint8_t channel, float value)
{
  if ((cursor + LPP_ANALOG_OUTPUT_SIZE) > maxsize)
  {
    return 0;
  }
  int16_t val = value * 100;
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_ANALOG_OUTPUT;
  buffer[cursor++] = val >> 8;
  buffer[cursor++] = val;

  return cursor;
}

uint8_t CayenneLPP::addLuminosity(uint8_t channel, uint16_t lux)
{
  if ((cursor + LPP_LUMINOSITY_SIZE) > maxsize)
  {
    return 0;
  }
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_LUMINOSITY;
  buffer[cursor++] = lux >> 8;
  buffer[cursor++] = lux;

  return cursor;
}

uint8_t CayenneLPP::addPresence(uint8_t channel, uint8_t value)
{
  if ((cursor + LPP_PRESENCE_SIZE) > maxsize)
  {
    return 0;
  }
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_PRESENCE;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addTemperature(uint8_t channel, float celsius)
{
  if ((cursor + LPP_TEMPERATURE_SIZE) > maxsize)
  {
    return 0;
  }
  int16_t val = celsius * 10;
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_TEMPERATURE;
  buffer[cursor++] = val >> 8;
  buffer[cursor++] = val;

  return cursor;
}

uint8_t CayenneLPP::addRelativeHumidity(uint8_t channel, float rh)
{
  if ((cursor + LPP_RELATIVE_HUMIDITY_SIZE) > maxsize)
  {
    return 0;
  }
  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_RELATIVE_HUMIDITY;
  buffer[cursor++] = rh * 2;

  return cursor;
}

uint8_t CayenneLPP::addAccelerometer(uint8_t channel, float x, float y, float z)
{
  if ((cursor + LPP_ACCELEROMETER_SIZE) > maxsize)
  {
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

uint8_t CayenneLPP::addBarometricPressure(uint8_t channel, float hpa)
{
  if ((cursor + LPP_BAROMETRIC_PRESSURE_SIZE) > maxsize)
  {
    return 0;
  }
  int16_t val = hpa * 10;

  buffer[cursor++] = channel;
  buffer[cursor++] = LPP_BAROMETRIC_PRESSURE;
  buffer[cursor++] = val >> 8;
  buffer[cursor++] = val;

  return cursor;
}

uint8_t CayenneLPP::addGyrometer(uint8_t channel, float x, float y, float z)
{
  if ((cursor + LPP_GYROMETER_SIZE) > maxsize)
  {
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

uint8_t CayenneLPP::addGPS(uint8_t channel, float latitude, float longitude, float meters)
{
  if ((cursor + LPP_GPS_SIZE) > maxsize)
  {
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