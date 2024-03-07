/*--------------------------------------------------------------------
  This code is free software:
  you can redistribute it and/or modify it under the terms of a Creative
  Commons Attribution-NonCommercial 4.0 International License
  (http://creativecommons.org/licenses/by-nc/4.0/) by
  Remko Welling (https://ese.han.nl/~rwelling/) E-mail: remko.welling@han.nl

  The program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  --------------------------------------------------------------------*/

/*!
 * \file LoRa_TX_RX_Cayenne_HAN.ino
 * \brief Sends packets on TTN using the LISSLoRa gadget.
 * Based on the original code from: https://github.com/YourproductSmarter/KISSLoRa-demo
 * \author Remko Welling (remko.welling@han.nl)
 * 
 * # Revision history
 * 
 * Version|Date      |Note
 * --------------------------------------
 * 1.0    |nov 2018  | Initial release
 * 1.1    |21-12-2018| Various additions
 * 1.2    | 5- 1-2019| added sleepmode for AVR and RN module, added documentation,
 * 1.3    |15- 1-2019| Changed addAnalogOutput to send right interval to cayenne,
 * 1.3.1  |27-03-2019| Corrected display of downlink setting of interval in serial communication
 * 4      |27-03-2019| Added functionality to set initial interval using the rotary encoder.
 * 4.1    |7-9-2020  | Sinitized code and comments
 */
/*!

## To use this board:
Install the USB drivers for the SparkFun boards, following the steps [for Windows](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-windows) or [for Linux and Mac](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-mac--linux).
In the Arduino IDE go to **File->Preferences->Additional Boards Manager URLs** and add: `https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json`
Go to **Tools->Board->Boards Manager**, search for **SparkFun AVR Boards** and click on **install**.
Go to **Tools->Board** and select **SparkFun Pro Micro**.
Go to **Tools->Processor** and select **ATmega32U4 (3.3V, 8MHz)**.

## Uploading Firmware
> The serial monitor must be closed before uploading code to the device.

1. Switch the KISS LoRa device off.
2. Plug it into your computer using a micro USB cable.
3. In the Arduino IDE make sure the correct **Tools->Board** (SparkFun Pro Micro) and **Tools->Processor** (ATmega 32U4, 8MHz) are selected.
4. Switch the device on and make sure the correct **Tools->Port** is selected.
5. Switch the device off again.
6. Press and hold the **Push Button** on the kiss device.
7. In the Arduino IDE, choose **Sketch->Upload**.
8. As soon as you see `PORTS {} / {} => {}` printed in the output window turn KISSLoRa on.
9. When the upload process continue past showing `PORTS {} / {} => {}`, you can release the push button.

 */

// Libraries and includes
//#include <TheThingsNetwork.h>   // include for TheThingsNetwork library
//#include <CayenneLPP.h>         // include for CayenneLPP library
#include "TheThingsNetwork.h" 
#include "CayenneLPP.h"
#include "SparkFun_Si7021_Breakout_Library.h" // include for temperature and humidity sensor
#include <Wire.h>
#include "KISSLoRa_sleep.h"     // Include to sleep MCU

#define RELEASE 4
#define USB_CABLE_CONNECTED (USBSTA&(1<<VBUS))

// defines Serial 
#define loraSerial Serial1
#define debugSerial Serial

// LoRaWAN TTN
#define freqPlan TTN_FP_EU868     ///< The KISS device should only be used in Europe

// Set your AppEUI and AppKey
// HAN KISS-xx: devEui is device specific
//const char *devEui = "70B3D57ED0065918";
//const char *appEui = "70B3D57ED0013DED"; 
//const char *appKey = "C5DAAB272E770448DD939CAB53C3BB9B"; //3C80CDEA19B9BFD182C1A244F11824DF

//const char *appEui = "70B3D57ED0013DED"; 
//const char *appKey = "2B8AF92FB36094A682B2CF99A4FC0CEF"; 

//const char *devEui = "70B3D57ED006599A";
//
const char *appEui = "0004A30B001EE766"; 
const char *appKey = "B6B97071E7CEF402A53C40AA3392257D"; //3C80CDEA19B9BFD182C1A244F11824DF

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);  // TTN object for LoRaWAN radio
//TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan, 9);  // TTN object for LoRaWAN radio using SF9

// Cayennel LPP
#define APPLICATION_PORT_CAYENNE  99   ///< LoRaWAN port to which CayenneLPP packets shall be sent
#define LPP_PAYLOAD_MAX_SIZE      51   ///< Maximum payload size of a LoRaWAN packet

#define LPP_CH_TEMPERATURE        0    ///< CayenneLPP CHannel for Temperature
#define LPP_CH_HUMIDITY           1    ///< CayenneLPP CHannel for Humidity sensor
#define LPP_CH_LUMINOSITY         2    ///< CayenneLPP CHannel for Luminosity sensor
#define LPP_CH_ROTARYSWITCH       3    ///< CayenneLPP CHannel for Rotary switch
#define LPP_CH_ACCELEROMETER      4    ///< CayenneLPP CHannel for Accelerometer
#define LPP_CH_BOARDVCCVOLTAGE    5    ///< CayenneLPP CHannel for Processor voltage
#define LPP_CH_PRESENCE           6    ///< CayenneLPP CHannel for Alarm
#define LPP_CH_SET_INTERVAL       20   ///< CayenneLPP CHannel for setting downlink interval
#define LPP_CH_SW_RELEASE         90   ///< 

#define ALARM             0x01  ///< Alarm state
#define SAFE              0x00  ///< No-alarm state

CayenneLPP lpp(LPP_PAYLOAD_MAX_SIZE); ///< Cayenne object for composing sensor message

// Sensors
Weather sensor;                  ///< temperature and humidity sensor

#define LIGHT_SENSOR_PIN  10     ///< Define for Analog input pin

// defines for LEDs
#define RGBLED_RED        12
#define RGBLED_GREEN      6
#define RGBLED_BLUE       11
#define LED_LORA          8

// defines for rotary encoder
#define ROTARY_PIN_0      5
#define ROTARY_PIN_1      13
#define ROTARY_PIN_2      9
#define ROTARY_PIN_3      0

// defines for pushbutton
#define BUTTON_PIN        7

// defines for accelerometer
#define ACC_RANGE         2       ///< Set up to read the accelerometer values in range -2g to +2g - valid ranges: �2G,�4G or �8G

float x,y,z;                      ///< Variables to hold acellerometer axis values.

// Set up application specific
#define REGULAR_INTERVAL  60000   ///< Regular transmission interval in ms

#define INTERVAL_ROTARY_MASK 0x07 ///< Binary mask: 0000 0111
#define INTERVAL_ROTARY_1 1       ///< 1 minute interval
#define INTERVAL_ROTARY_2 2       ///< 5 minutes interval
#define INTERVAL_ROTARY_3 3       ///< 15 minutes interval
#define INTERVAL_ROTARY_4 4       ///< 60 minutes interval

#define INTERVAL_1        60000   ///< 1 minute interval
#define INTERVAL_2        300000  ///< 5 minutes interval
#define INTERVAL_3        900000  ///< 15 minutes interval
#define INTERVAL_4        3600000 ///< 60 minutes interval

uint32_t currentInterval = REGULAR_INTERVAL;
uint32_t nextInterval    = REGULAR_INTERVAL;

bool alarm = { false };           ///< Variable to hold alarm state when set in ISR from button.

// \brief setup
void setup(){
  KISSLoRa_sleep_init();
  
  // Initlialize serial
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Initialize LED outputs
  pinMode(RGBLED_RED,   OUTPUT);
  pinMode(RGBLED_GREEN, OUTPUT);
  pinMode(RGBLED_BLUE,  OUTPUT);
  pinMode(LED_LORA,     OUTPUT);

  // initialize rotary encoder
  //Set pins as inputs
  pinMode(ROTARY_PIN_0, INPUT);
  pinMode(ROTARY_PIN_1, INPUT);
  pinMode(ROTARY_PIN_2, INPUT);
  pinMode(ROTARY_PIN_3, INPUT);
  //Disable pullup resistors
  digitalWrite(ROTARY_PIN_0, 0);
  digitalWrite(ROTARY_PIN_1, 0);
  digitalWrite(ROTARY_PIN_2, 0);
  digitalWrite(ROTARY_PIN_3, 0);

  // configure button
  pinMode(BUTTON_PIN, INPUT);     //Set pin as inputs
  digitalWrite(BUTTON_PIN, 0);    //Disable pullup resistors
  //Attach an interrupt to the button pin - fire when button pressed down.
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressedISR, FALLING);

  //Initialize the I2C Si7021 sensor
  sensor.begin();

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Switch off leds
  digitalWrite(RGBLED_RED,   HIGH);  //switch RGBLED_RED LED off
  digitalWrite(RGBLED_GREEN, HIGH);  //switch RGBLED_GREEN LED off
  digitalWrite(RGBLED_BLUE,  HIGH);  //switch RGBLED_BLUE LED off
  digitalWrite(LED_LORA,     HIGH);  //switch LED_LORA LED off

  Wire.begin();
  initAccelerometer();
  setAccelerometerRange(ACC_RANGE); 

  // Initialize LoRaWAN radio
  digitalWrite(RGBLED_RED, LOW);    //switch RGBLED_RED LED on
    
  ttn.onMessage(message);           // Set callback for incoming messages
  ttn.reset(true);                  // Reset LoRaWAN mac and enable ADR
  
  debugSerial.println(F("-- STATUS"));
  ttn.showStatus();

  debugSerial.println(F("-- JOIN"));
  ttn.join(appEui, appKey);

  // initilize interval from rotary switch
  nextInterval = getInitialInterval((uint8_t)getRotaryPosition());

  digitalWrite(RGBLED_RED, HIGH);   //switch RGBLED_RED LED off when join succeeds
}

// \brief mainloop
void loop(){
  debugSerial.println(F("-- LOOP"));

  if(currentInterval != nextInterval){
    debugSerial.print("Next interval set to: " + String(nextInterval/1000));
    debugSerial.println(F(" Seconds"));    
  }
  currentInterval = nextInterval;
  
  digitalWrite(RGBLED_RED, HIGH);   //switch RGBLED_RED LED off
  digitalWrite(RGBLED_GREEN, HIGH); //switch RGBLED_GREEN LED off
  digitalWrite(RGBLED_BLUE, HIGH);  //switch RGBLED_BLUE LED off

  // Measure Relative Humidity from the Si7021
  float humidity = sensor.getRH();
  debugSerial.print(F("Humidity: "));
  debugSerial.print(humidity);
  debugSerial.println(F(" %RH."));

  // Measure Temperature from the Si7021
  float temperature = sensor.getTemp();
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTemp() instead with readTemp()
  debugSerial.print(F("Temperature: "));
  debugSerial.print(temperature);
  debugSerial.println(F(" Degrees."));

  // Measure luminosity
  float luminosity = get_lux_value();
  Serial.print(F("Ambient light: "));
  Serial.print(luminosity);
  Serial.println(F(" lux"));

  // get rotary encode position
  uint8_t rotaryPosition = (uint8_t)getRotaryPosition();
  Serial.print(F("Rotary encoder position: "));
  Serial.println(rotaryPosition);

  /// get accelerometer
  getAcceleration(&x, &y, &z);
  Serial.print(F("Acceleration:\tx="));
  Serial.print(x);
  Serial.print(F("g\n\t\ty="));
  Serial.print(y);
  Serial.print(F("g\n\t\tz="));
  Serial.print(z);
  Serial.println(F("g"));

  /// get VDD form RN module
  float vdd = (float)ttn.getVDD()/1000;
  Serial.print(F("RN2483 voltage: "));
  Serial.print(vdd);
  Serial.println(F(" Volt"));
  
  // Wake RN2483 
  ttn.wake();
  
  // Compose Cayenne message
  lpp.reset();    // reset cayenne object

  // add sensor values to cayenne data package
  lpp.addTemperature(LPP_CH_TEMPERATURE, temperature);
  lpp.addRelativeHumidity(LPP_CH_HUMIDITY, humidity);
  lpp.addLuminosity(LPP_CH_LUMINOSITY, luminosity);
  //lpp.addDigitalInput(LPP_CH_ROTARYSWITCH, rotaryPosition);
  lpp.addDigital(LPP_CH_ROTARYSWITCH, rotaryPosition, LPP_DIGITAL_INPUT);
  lpp.addAccelerometer(LPP_CH_ACCELEROMETER, x, y, z);
  lpp.addPresence(LPP_CH_PRESENCE, SAFE);
  //lpp.addAnalogInput(LPP_CH_BOARDVCCVOLTAGE, vdd);
  //lpp.addAnalogOutput(LPP_CH_SET_INTERVAL, (float)currentInterval/1000);
  lpp.addAnalog(LPP_CH_BOARDVCCVOLTAGE, vdd, LPP_ANALOG_INPUT);
  lpp.addAnalog(LPP_CH_SET_INTERVAL, static_cast<float>(currentInterval)/1000, LPP_ANALOG_OUTPUT);
  
  
  digitalWrite(LED_LORA, LOW);  //switch LED_LORA LED on

  // send cayenne message on port 99
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize(), APPLICATION_PORT_CAYENNE);

  // Set RN2483 to sleep mode
  ttn.sleep(currentInterval - 100);
  // This delay is not optional, try to remove it
  // and say bye bye to your RN2483 sleep mode
  delay(50);

  digitalWrite(LED_LORA, HIGH);  //switch LED_LORA LED off
  
  // Set KISSLoRa to sleep.
  sleep(currentInterval);
}

/// \brief function called at RX message
/// \param payload pointer to received payload
/// \param size payload size
/// \param port Application port
void message(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println(F("-- MESSAGE"));
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ": ");

  switch(port)
  {
    case 99:
      if(payload[0] == 0x14){
        uint32_t tempValue = 0;
        tempValue |= payload[1] << 8;
        tempValue |= payload[2];
        nextInterval = tempValue * 10;
        debugSerial.print("New interval: " + String(nextInterval/1000));
        debugSerial.println(F(" Seconds"));
        digitalWrite(RGBLED_BLUE, !digitalRead(RGBLED_BLUE));
      }else{
        debugSerial.println(F("Wrong downlink message."));
      }
      break;
    default:
      {
        for (int i = 0; i < size; i++){
          debugSerial.print(" " + String(payload[i]));
        }
        debugSerial.println();
      }
      //Toggle green LED when a message is received
      digitalWrite(RGBLED_GREEN, !digitalRead(RGBLED_GREEN));
      break;
  }
}

/// \brief read luminosty value from sensor
///  Get the lux value from the APDS-9007 Ambient Light Photo Sensor
/// \return luminosity in Lux.
float get_lux_value(void){
  int digital_value = analogRead(LIGHT_SENSOR_PIN);
  double vlux = digital_value * (2.56/1023.0); //lux value in volts
  double ilux = (vlux / 56) * 1000;            //lux value in micro amperes
  double lux = pow(10, (ilux / 10));           //Convert ilux to Lux value
  return (float)lux;                        //Return Lux value as value without decimal
}

/// \brief read rotary switch value
///  Poll the rotary switch
/// \retval binary representation of rotarty switch position ( 0 to 9)
int8_t getRotaryPosition(){
  int8_t value = 0;
  if (digitalRead(ROTARY_PIN_0)) {value |= 1 << 0;}
  if (digitalRead(ROTARY_PIN_1)) {value |= 1 << 1;}
  if (digitalRead(ROTARY_PIN_2)) {value |= 1 << 2;}
  if (digitalRead(ROTARY_PIN_3)) {value |= 1 << 3;}
  return value;
}

/// \brief function called at interrupt generated by pushbutton
void buttonPressedISR(){
  alarm = true;
}

/// \brief Write one register to the acceleromter
/// \param REG_ADDRESS address of register
/// \brief DATA to be written to that address.
void writeAccelerometer(unsigned char REG_ADDRESS, unsigned char DATA){
  Wire.beginTransmission(0x1D);
  Wire.write(REG_ADDRESS);
  Wire.write(DATA);
  Wire.endTransmission();
}

/// \brief Read one register from the accelerometer
/// \param REG_ADDRESS address of registry to be read.
/// \return Value at given registry address
uint8_t readAccelerometer(unsigned char REG_ADDRESS){
  uint8_t resp;
  Wire.beginTransmission(0x1D);
  Wire.write(REG_ADDRESS);
  Wire.endTransmission(false);
  Wire.requestFrom(0x1D, 1);
  resp = Wire.read();
  return resp;
}

/// \brief Configure and activate the FXLS8471Q Accelerometer 
static void initAccelerometer(void){
  //Check if the chip responds to the who-am-i command, should return 0x6A (106)
  if (readAccelerometer(0x0D) == 106){
    //Configure FXLS8471Q CTRL_REG1 register
    //Set f_read bit to activate fast read mode
    //Set active bit to put accelerometer in active mode
    writeAccelerometer(0x2A, 0x03);  
  }else{
    Serial.println(F("--- I2C Accelerometer not initialized"));
  } 
}

/// \brief Set the range of the FXLS8471Q Accelerometer
/// \param range_g Range to be set. valid ranges: 2G,4G or 8G
static void setAccelerometerRange(uint8_t range_g){
  uint8_t range_b;
  switch(range_g){
    case 2:
      range_b = 0;
      break;
    
    case 4:
      range_b = 1;
      break;
      
    case 8:
      range_b = 2;
      break;
      
    default:
      range_b = 0;
      break;
  }
  writeAccelerometer(0x0E, range_b);
}

/// \brief Read the acceleration from the accelerometer
/// \param x pointer to x-value
/// \param y pointer to y-value
/// \param z pointer to z-value
void getAcceleration(float *x, float *y, float *z){
  // Resource: https://github.com/sparkfun/MMA8452_Accelerometer/blob/master/Libraries/Arduino/src/SparkFun_MMA8452Q.cpp
  // Read the acceleration from registers 1 through 6 of the MMA8452 accelerometer.
  // 2 registers per axis, 12 bits per axis.
  // Bit-shifting right does sign extension to preserve negative numbers.
  *x = ((short)(readAccelerometer(1)<<8 | readAccelerometer(2))) >> 4;
  *y = ((short)(readAccelerometer(3)<<8 | readAccelerometer(4))) >> 4;
  *z = ((short)(readAccelerometer(5)<<8 | readAccelerometer(6))) >> 4;

  // Scale 12 bit signed values to units of g. The default measurement range is �2g.
  // That is 11 bits for positive values and 11 bits for negative values.
  // value = (value / (2^11)) * 2
  *x = (float)*x / (float)(1<<11) * (float)(ACC_RANGE);
  *y = (float)*y / (float)(1<<11) * (float)(ACC_RANGE);
  *z = (float)*z / (float)(1<<11) * (float)(ACC_RANGE);
}

/// \brief Sleep until a given time has passed, or if the push button is pressed, or if rotary switch is changed
/// During sleep the function will observe for interrupt by button.
/// \param delay_time_ms time in ms to sleep.
static void sleep(uint32_t delay_time_ms){
  //Loop until delay is over, or if the push button is pressed, or if rotary switch is changed
  while (delay_time_ms)
  {
    if(!USB_CABLE_CONNECTED){
      KISSLoRa_sleep_delay_ms(100);
    }else{
      delay(100);
    }

    // Event: Send acknowledged message at alarm.
    if(alarm){
      debugSerial.println(F("-- ALARM!"));
      digitalWrite(RGBLED_RED, LOW);  //switch RGBLED_RED LED on

      // Wake RN2483 
      ttn.wake();
        
      lpp.reset();
      lpp.addPresence(LPP_CH_PRESENCE, ALARM);
      //lpp.addDigitalInput(LPP_CH_SW_RELEASE, RELEASE);
      lpp.addDigital(LPP_CH_SW_RELEASE, RELEASE, LPP_DIGITAL_INPUT);
  
      // Send it off
      ttn.sendBytes(lpp.getBuffer(), lpp.getSize(), APPLICATION_PORT_CAYENNE, true);

      // Set RN2483 to sleep mode
      ttn.sleep(delay_time_ms - 2200);
      // This delay is not optional, try to remove it
      // and say bye bye to your RN2483 sleep mode
      delay(50);
      
      alarm = false;
      
      digitalWrite(RGBLED_RED, HIGH);  //switch RGBLED_RED LED off
    }

    if(delay_time_ms > 100){
      delay_time_ms -= 100;
    }else{
      delay_time_ms = 0;
    }
  }
}

/// \brief Determine interval in ms using rotary value
/// \pre This function is only using bits 4, 2, and 1 while ignoring bit 8.
/// Set this using define INTERVAL_ROTARY_MASK
/// \param rotaryValue actual value from rotary encoder.
/// \return interval in ms
uint32_t getInitialInterval(uint8_t rotaryValue){
  uint32_t intervalMs = REGULAR_INTERVAL;
  
  rotaryValue &= INTERVAL_ROTARY_MASK;
  switch(rotaryValue){
    case INTERVAL_ROTARY_1:
      intervalMs = INTERVAL_1;
      break;

    case INTERVAL_ROTARY_2:
      intervalMs = INTERVAL_2;
      break;

    case INTERVAL_ROTARY_3:
      intervalMs = INTERVAL_3;
      break;

    case INTERVAL_ROTARY_4:
      intervalMs = INTERVAL_4;
      break;

    default:
      intervalMs = REGULAR_INTERVAL;
      break;
  }
  return intervalMs;
}
