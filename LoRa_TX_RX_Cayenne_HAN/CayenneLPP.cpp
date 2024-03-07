// Adapted from https://developer.mbed.org/teams/myDevicesIoT/code/Cayenne-LPP/

// Copyright © 2017 The Things Network
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#include "CayenneLPP.h"

CayenneLPP::CayenneLPP(uint8_t size) : maxsize(size)
{
  buffer = (uint8_t *)malloc(size);
  cursor = 0;
}

CayenneLPP::~CayenneLPP(void)
{
  free(buffer);
}

void CayenneLPP::reset(void)
{
  cursor = 0;
}

uint8_t CayenneLPP::getSize(void)
{
  return cursor;
}

uint8_t *CayenneLPP::getBuffer(void)
{
  //    uint8_t[cursor] result;
  //    memcpy(result, buffer, cursor);
  //    return result;
  return buffer;
}

uint8_t CayenneLPP::copy(uint8_t *dst)
{
  memcpy(dst, buffer, cursor);
  return cursor;
}

uint8_t CayenneLPP::addBoolean(uint8_t channel, bool value)
{
  return addByte(channel, static_cast<uint8_t>(value));
}

uint8_t CayenneLPP::addByte(uint8_t channel, uint8_t value) 
{
  return addOneByteData(channel, LPP_GENERIC_16BIT, value);
}

uint8_t CayenneLPP::add16Bit(uint8_t channel, uint16_t value) 
{
  return addTwoByteData(channel, LPP_GENERIC_16BIT, value);
}

uint8_t CayenneLPP::add32Bit(uint8_t channel, uint32_t value) 
{
  return addFourByteData(channel, LPP_GENERIC_32BIT, value);
}

uint8_t CayenneLPP::addFloat(uint8_t channel, float value) 
{
  return addFourByteData(channel, LPP_GENERIC_FLOAT, *reinterpret_cast<uint32_t *>(&value));
}

uint8_t CayenneLPP::addVersion(uint8_t version) 
{
  return addOneByteData(0, LPP_VERSION, version);
}

uint8_t CayenneLPP::addOneByteData(uint8_t channel, uint8_t type, uint8_t value) 
{
  if ((cursor + 3) > maxsize) {
    return 0;
  }

  buffer[cursor++] = type;
  buffer[cursor++] = channel;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addTwoByteData(uint8_t channel, uint8_t type, uint16_t value) 
{
  if ((cursor + 4) > maxsize) {
    return 0;
  }
  buffer[cursor++] = type;
  buffer[cursor++] = channel;
  buffer[cursor++] = value >> 8;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addFourByteData(uint8_t channel, uint8_t type, uint32_t value) 
{
  if ((cursor + 5) > maxsize) {
    return 0; // Buffer overflow
  }

  buffer[cursor++] = type;
  buffer[cursor++] = channel;
  buffer[cursor++] = (value >> 24) & 0xFF;
  buffer[cursor++] = (value >> 16) & 0xFF;
  buffer[cursor++] = (value >> 8) & 0xFF;
  buffer[cursor++] = value & 0xFF;

  return cursor;
}

uint8_t CayenneLPP::addDigital(uint8_t channel, uint8_t value, uint8_t type)
{
  uint8_t size = (type == LPP_DIGITAL_INPUT) ? LPP_DIGITAL_INPUT_SIZE : LPP_DIGITAL_OUTPUT_SIZE;

  if ((cursor + size) > maxsize)
  {
    return 0;
  }

  buffer[cursor++] = channel;
  buffer[cursor++] = type;
  buffer[cursor++] = value;

  return cursor;
}

uint8_t CayenneLPP::addAnalog(uint8_t channel, float value, uint8_t type)
{
  uint8_t size = (type == LPP_ANALOG_INPUT) ? LPP_ANALOG_INPUT_SIZE : LPP_ANALOG_OUTPUT_SIZE;
  
  if ((cursor + size) > maxsize)
  {
    return 0;
  }

  int16_t val = value * 100;
  buffer[cursor++] = channel;
  buffer[cursor++] = type;
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
