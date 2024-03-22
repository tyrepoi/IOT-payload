/**
 * @file  CayenneLPP.h
 * @brief Library for creating Cayenne Low Power Payload (LPP) packets.
 * @note  This library requires a working knowledge of the Cayenne Low Power Protocol (LPP) specification.
 * @copyright (c) 2024, Leon Nguyen and Len Verploegen
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @author  Leon Nguyen and Len Verpleogen
 */
#ifndef _CAYENNE_LPP_H_
#define _CAYENNE_LPP_H_

#include <Arduino.h>

#define LPP_DIGITAL_INPUT 0         /// \ Identifier for digital input data type (1 Byte)
#define LPP_DIGITAL_OUTPUT 1        /// \ Identifier for digital output data type (1 Byte)
#define LPP_ANALOG_INPUT 2          /// \ Identifier for analog input data type (2 bytes and is scaled by a factor of 0.01 (signed)
#define LPP_ANALOG_OUTPUT 3         /// \ Identifier for analog output data type (2 bytes and is scaled by a factor of 0.01 (signed))
#define LPP_ADDBIT 4                /// \ Identifier for additional bit data type (1 Byte)
#define LPP_ADDBYTE 5               /// \ Identifier for additional byte data type (1 Byte)
#define LPP_ADD2BYTES 6             /// \ Identifier for additional 2-byte data type (2 bytes)
#define LPP_ADD4BYTES 7             /// \ Identifier for additional 4-byte data type (4 bytes)
#define LPP_ADDFLOAT 8              /// \ Identifier for additional floating-point data type
#define LPP_CUSTOMBYTE 9
#define LPP_LUMINOSITY 101          /// \ Identifier for luminosity data type (2 bytes, 1 lux unsigned)
#define LPP_PRESENCE 102            /// \ Identifier for presence data type (1 byte, 1)
#define LPP_TEMPERATURE 103         /// \ Identifier for temperature data type (2 bytes, 0.1°C signed)
#define LPP_RELATIVE_HUMIDITY 104   /// \ Identifier for relative humidity data type (1 byte, 0.5% unsigned)
#define LPP_ACCELEROMETER 113       /// \ Identifier for accelerometer data type (2 bytes per axis, 0.001G)
#define LPP_BAROMETRIC_PRESSURE 115 /// \ Identifier for barometric pressure data type (2 bytes 0.1 hPa Unsigned)
#define LPP_GYROMETER 134           /// \ Identifier for gyrometer data type (2 bytes per axis, 0.01 °/s)
#define LPP_GPS 136                 /// \ Identifier for GPS data type (3 byte lon/lat 0.0001 °, 3 bytes alt 0.01 meter)

// The total number of bytes required for the Data ID, Data Type, and Data Size
#define LPP_DIGITAL_INPUT_SIZE 3       
#define LPP_DIGITAL_OUTPUT_SIZE 3     
#define LPP_ANALOG_INPUT_SIZE 4        
#define LPP_ANALOG_OUTPUT_SIZE 3     
#define LPP_LUMINOSITY_SIZE 4         
#define LPP_PRESENCE_SIZE 3         
#define LPP_TEMPERATURE_SIZE 3        
#define LPP_RELATIVE_HUMIDITY_SIZE 3   
#define LPP_ACCELEROMETER_SIZE 8       
#define LPP_BAROMETRIC_PRESSURE_SIZE 4
#define LPP_GYROMETER_SIZE 8           
#define LPP_GPS_SIZE 11              

/**
 * @brief Cayenne Low Power Protocol (LPP) packet builder class.
 * This class provides methods to build Cayenne LPP packets for sending sensor data over LoRaWAN
 */
class CayenneLPP
{
public:
	/**
	 * @brief Constructor for CayenneLPP class.
	 * @param size Maximum size of the LPP packet.
	 */
	CayenneLPP(uint8_t size);

	/**
	 * @brief Destructor for CayenneLPP class.
	 */
	~CayenneLPP();

	/**
	 * @brief Reset the LPP packet buffer.
	 */
	void reset(void);

	/**
	 * @brief Get the size of the LPP packet buffer.
	 * @return Size of the LPP packet buffer.
	 */
	uint8_t getSize(void);

	/**
	 * @brief Get a pointer to the LPP packet buffer.
	 * @return Pointer to the LPP packet buffer.
	 */
	uint8_t *getBuffer(void);

	/**
	 * @brief Copy the LPP packet buffer to an external buffer.
	 * @param buffer External buffer to copy the LPP packet buffer into.
	 * @return Number of bytes copied.
	 */
	uint8_t copy(uint8_t *buffer);

	/**
	 * @brief Add a single bit value to the LPP packet.
	 * @param channel Channel number.
	 */
	uint8_t addBit(uint8_t channel, uint8_t value);

  uint8_t addCustomByte(uint8_t channel, uint8_t type, float value, uint16_t resolution, uint8_t num_bytes);

	/**
	 * @brief Add a byte value to the LPP packet.
	 * @param channel Channel number.
   * @param implemented function addByte for the functions (addDigitalInput and addDigitalOutput).
	 * @param value Byte value.
	 */ 
  uint8_t addByte(uint8_t channel, uint8_t type, float value, uint16_t resolution);
	
	/**
	 * @brief Add a two-byte value to the LPP packet.
	 * @param channel Channel number.
   * @param implemented function add2Bytes for the functions (temperature, addLuminosity, addAnalogInput, addAnalogOutput)
	 * @param value Two-byte value.
	 */
  uint8_t add2Bytes(uint8_t channel, uint8_t type, float value, uint16_t resolution);

	/**
	 * @brief Add a four-byte value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Four-byte value.
	 */  
	uint8_t add4Bytes(uint8_t channel, uint8_t type, float value, uint16_t resolution); 

	/**
	 * @brief Add a floating-point value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Floating-point value.
	 */
	uint8_t addFloat(uint8_t channel, float value, uint16_t resolution); 

  /**
   * @brief Add a 3float value to the LPP packet.
   * @param channel Channel number.
   * @param value Floating-point value.
   */
  uint8_t add3Float(uint8_t channel, uint8_t type, float x, float y, float z, uint16_t resolution);

	/**
	 * @brief Add a digital input value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Digital input value.
   * @param bitSize Size of the one byte field (1 to 8).
	 */
	//uint8_t addDigitalInput(uint8_t channel, uint8_t value);

	/**
	 * @brief Add a digital output value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Digital output value.
	 */
	//uint8_t addDigitalOutput(uint8_t channel, uint8_t value);

	/**
	 * @brief Add an analog input value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Analog input value.
	 */
	uint8_t addAnalogInput(uint8_t channel, float value);

	/**
	 * @brief Add an analog output value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Analog output value.
	 */
	uint8_t addAnalogOutput(uint8_t channel, float value);

	/**
	 * @brief Add luminosity value to the LPP packet.
	 * @param channel Channel number.
	 * @param lux Luminosity value.
	 */
	uint8_t addLuminosity(uint8_t channel, uint16_t lux);

	/**
	 * @brief Add presence value to the LPP packet.
	 * @param channel Channel number.
	 * @param value Presence value.
	 */
	uint8_t addPresence(uint8_t channel, uint8_t value);

	/**
	 * @brief Add temperature value to the LPP packet.
	 * @param channel Channel number.
	 * @param celsius Temperature value in Celsius.
	 */
	uint8_t addTemperature(uint8_t channel, float celsius);

	/**
	 * @brief Add relative humidity value to the LPP packet.
	 * @param channel Channel number.
	 * @param rh Relative humidity value.
	 */
	uint8_t addRelativeHumidity(uint8_t channel, float rh);

	/**
	 * @brief Add accelerometer data to the LPP packet.
	 * @param channel Channel number.
	 * @param x Acceleration value along the X-axis.
	 * @param y Acceleration value along the Y-axis.
	 * @param z Acceleration value along the Z-axis.
	 */
	uint8_t addAccelerometer(uint8_t channel, float x, float y, float z);

	/**
	 * @brief Add barometric pressure value to the LPP packet.
	 * @param channel Channel number.
	 * @param hpa Barometric pressure value in hPa.
	 */
	uint8_t addBarometricPressure(uint8_t channel, float hpa);

	/**
	 * @brief Add gyrometer data to the LPP packet.
	 * @param channel Channel number.
	 * @param x Gyrometer value along the X-axis.
	 * @param y Gyrometer value along the Y-axis.
	 * @param z Gyrometer value along the Z-axis.
	 */
	uint8_t addGyrometer(uint8_t channel, float x, float y, float z);

	/**
	 * @brief Add GPS data to the LPP packet.
	 * @param channel Channel number.
	 * @param latitude Latitude value.
	 * @param longitude Longitude value.
	 * @param meters Altitude value in meters.
	 */
	uint8_t addGPS(uint8_t channel, float latitude, float longitude, float meters);

private:

	/**
	 * @brief Pointer to the LPP packet buffer.
	 * This variable holds the memory address of the LPP packet buffer.
	 */
	uint8_t *buffer;

	/**
	 * @brief Maximum size of the LPP packet buffer.
	 * This variable stores the maximum size of the LPP packet buffer.
	 */
	uint8_t maxsize; 

	/**
	 * @brief Cursor indicating the current position in the buffer.
	 * This variable represents the current position within the LPP packet buffer.
	 */
	uint8_t cursor; 
};

#endif
