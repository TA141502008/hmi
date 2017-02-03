# HMI #

## Function of HMI ##
* Receive measurement values from RPM.
* Displays the values on the LCD.
* Storing the values to the SD card.
* Displays the value status whether normal or not by: (1) the color of the LED; and (2) minus-sign or plus-sign on the LCD.
* It displays the condition of HMI component by LED color.
* Give feedback alarm signal and sending SMS if there is a condition that parameter value passes the normal limit.

## Microcontroller Board ##
Arduino Mega 2560 R3 (IDE used is Arduino 1.6.4)

## Pin Configuration ##

### Keypad 4x4 ###
* Row 0    = pin 29
* Row 1    = pin 28
* Row 2    = pin 27
* Row 3    = pin 26
* Column 0 = pin 25
* Column 1 = pin 24
* Column 2 = pin 23
* Column 3 = pin 22

### LCD 16x4 ###
* 1  (VSS)  = -
* 2  (VDD)  = 5V
* 3  (V0)   = GND
* 4  (RS)   = pin 12
* 5  (RW)   = GND
* 6  (E)    = pin 11
* 7  (DB0)  = -
* 8  (DB1)  = -
* 9  (DB2)  = -
* 10 (DB3)  = -
* 11 (DB4)  = pin 5
* 12 (DB5)  = pin 4
* 13 (DB6)  = pin 3
* 14 (DB7)  = pin 2
* 15 (LEDA) = 3.3V
* 16 (LEDK) = GND

### Tiny RTC I2C Modules ###
* SQ  = -
* DS  = -
* SCL = pin 21 (SCL)
* SDA = pin 20 (SDA)
* VCC = 5V
* GND = GND

### SD Card Module ###
* GND  = -
* 3V3  = -
* 5V   = 5V
* CS   = pin 53
* MOSI = pin 51 (MOSI)
* SCK  = pin 52 (SCK)
* MISO = pin 50 (MISO)
* GND  = GND

### Receiver 433 MHz ###
* DATA1 = pin 6
* DATA2 = -
* VCC   = 5V
* GND   = GND

### LED Component Board ###
* Rx_Green  = pin 7
* RTC_Green = pin 8
* RTC_Red   = pin 14
* SD_Green  = pin 15
* SD_Yellow = pin 16
* SD_Red    = pin 17
* Ground    = GND

### LED Sensor Board ###
* Temperature_Green = pin 30
* Temperature_Red   = pin 31
* pH_Green          = pin 32
* pH_Red            = pin 33
* DO_Green          = pin 34
* DO_Red            = pin 36
* Turbidity_Green   = pin 38
* Turbidity_Red     = pin 40
* Salinity_Green    = pin 42
* Salinity_Red      = pin 44
* Depth_Green       = pin 46
* Depth_Red         = pin 48
* Ground            = GND

### Relay ###
* VCC    = 5V
* Alarm  = 18 (CH2)
* Kincir = 19 (CH1)
* GND    = GND

### GSM Module ###
* 5VR (RX) = pin 9
* 5VT (TX) = pin 10
* GND*  = GND
