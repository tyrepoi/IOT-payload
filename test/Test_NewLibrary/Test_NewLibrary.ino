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
#include "TheThingsNetwork.h" 
#include "CayenneLPP_NewLibrary.h" // include for CayenneLPP_NewLibrary
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


// HAN KISS-xx: devEui is device specific
//const char *devEui = "70B3D57ED0065918";
//const char *appEui = "70B3D57ED0013DED"; 
//const char *appKey = "C5DAAB272E770448DD939CAB53C3BB9B"; //3C80CDEA19B9BFD182C1A244F11824DF

// Set your AppEUI and AppKey
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

#define ALARM                     0x01 ///< Alarm state
#define SAFE                      0x00 ///< No-alarm state

CayenneLPP lpp(LPP_PAYLOAD_MAX_SIZE);  ///< Cayenne object for composing sensor message

// Sensors
Weather sensor;                        ///< temperature and humidity sensor

#define LIGHT_SENSOR_PIN  10           ///< Define for Analog input pin

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
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Switch off leds
  digitalWrite(RGBLED_RED,   HIGH);  //switch RGBLED_RED LED off
  digitalWrite(RGBLED_GREEN, HIGH);  //switch RGBLED_GREEN LED off
  digitalWrite(RGBLED_BLUE,  HIGH);  //switch RGBLED_BLUE LED off
  digitalWrite(LED_LORA,     HIGH);  //switch LED_LORA LED off

  Wire.begin();

  // Initialize LoRaWAN radio
  digitalWrite(RGBLED_RED, LOW);    //switch RGBLED_RED LED on
    
  ttn.onMessage(message);           // Set callback for incoming messages
  ttn.reset(true);                  // Reset LoRaWAN mac and enable ADR
  
  debugSerial.println(F("-- STATUS"));
  ttn.showStatus();

  debugSerial.println(F("-- JOIN"));
  ttn.join(appEui, appKey);

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

  float humidity = 53.5;
  debugSerial.print(F("Humidity: "));
  debugSerial.print(humidity);
  debugSerial.println(F(" %RH."));

  float temperature = 21.7;
  debugSerial.print(F("Temperature: "));
  debugSerial.print(temperature);
  debugSerial.println(F(" Degrees."));

  float luminosity = 236;
  Serial.print(F("Ambient light: "));
  Serial.print(luminosity);
  Serial.println(F(" lux"));

  // Wake RN2483 
  ttn.wake();
  
  // Compose Cayenne message
  lpp.reset();    // reset cayenne object
  
  // add sensor values to cayenne data package
  //lpp.addByte(LPP_CH_ADDBYTE, one);

  uint32_t big = 309;
  //lpp.add4Bytes(LPP_CH_ADD4BYTES, big);

  uint8_t bufesize = 3;
  uint8_t inputsa = 1;

  lpp.addWord(LPP_CH_TEMPERATURE, LPP_TEMPERATURE, temperature, 10);
  lpp.addByte(LPP_CH_HUMIDITY, LPP_RELATIVE_HUMIDITY, humidity, 2);
  lpp.addWord(LPP_CH_LUMINOSITY, LPP_LUMINOSITY, luminosity, 1);

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
      lpp.addByte(LPP_CH_PRESENCE, LPP_PRESENCE, ALARM, 1);
      lpp.addByte(LPP_CH_SW_RELEASE, LPP_DIGITAL_INPUT, RELEASE, 1);
  
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
