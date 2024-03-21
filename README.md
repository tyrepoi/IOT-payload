# Payload encoder -decoder library assignment
In semester 6 of the Embedded Systems Engineering program, students focus on the Internet of Things (IoT). In this context, LoRaWAN technology is used to transmit data from the KISS LoRa device. The task is to develop a solution for the improvement of the encoding and decoding of the payloads used in this system.

## The assignment
A popular library for constructing a payload with LoRaWAN is the CayenneLPP library, which is part of The Things Network (TTN) library. The CayenneLPP library is a flexible solution to use in rapid prototyping, but is not optimized for payload size. To promote flexibility, the payload uses an implicit payload composition scheme that results in additional overhead in the payload. There is also significant overhead in the code 
The assignment involves developing an innovative solution for improving payload encoding and decoding processes. 

## Kiss LoRa Device
The KISS LoRa was a gadget that was issued to visitors to the Dutch electonics fair <a rel="EandA" href="https://fhi.nl/eabeurs/kiss-lora-ea-2017-gadget/">Electroncs & Applications</a> and produced in a serie of aproximately 2000 devices. The purpose was to attract visitors to <a rel="TTN" href="https://www.thethingsnetwork.org/">The Things Network</a> and to propmote companies that participated in producing the KISS LoRa.

![KISS LoRa](images/KISSLoRa-1024x400.png)
Image source: https://diystuff.nl/lorawan/kisslora/

The board contains: 

 - Microchip RN2483 Low-Power Long Range LoRa™ Technology Transceiver Module.
 - ATmega32u4 8-bit Microcontroller with 32K bytes of ISP Flash and USB Controller.
 - FXLS8471Q 3-Axis Linear Accelerometer.
 - APDS-9007 Ambient Light Photo Sensor with Logarithmic Current Output.
 - SI7021-A20 I2C Humidity and Temperature sensor.

## License
All copyrights belong to their respective owners and are mentioned there were known.

This repository is part of the IoT classes delivered at HAN Engineering ELT.

This repository is free: You may redistribute it and/or modify it under the terms of a Creative  Commons Attribution-NonCommercial 4.0 International License  (http://creativecommons.org/licenses/by-nc/4.0/) by Leon Nguyen and Len Verploegen

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
