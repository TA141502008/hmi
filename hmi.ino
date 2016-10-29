/****************************************************************************
=============================================================================
Project        : Experiment
                Mixing LCD 16x4, Keypad 4x4, RTC, and SD Card Module
Version        : 1.3
Date Created   : August 27, 2015
Date Modified  : September 10, 2015
Author         : Baharuddin Aziz (http://baha.web.id)
Company        : Department of Electrical Engineering
                 School of Electrical Engineering and Informatics
                 Bandung Institute of Technology (ITB)
Summary        : 1. This program for implement 4x4 Matrix Keypad
                    to LCD 16x4 that connected to Arduino board
                    with RTC module.
                 2. Every keypress will be display on LCD 16x4, and
                    also the date and clock of the day.
                 3. Every keypress also will be write on file in SD Card.
                 4. Every file only available one year.
                    After that, the file will be deleted.
                 5.
                 6.
                 7.
                 8.
=============================================================================
                         Additional Info
-----------------------------------------------------------------------------
> Microcontroller Board :
   Arduino Mega 2560 R3 (IDE used is Arduino 1.6.4)
> Pin Configuration of :
   Keypad 4x4 :
     Row 0    = pin 29
     Row 1    = pin 28
     Row 2    = pin 27
     Row 3    = pin 26
     Column 0 = pin 25
     Column 1 = pin 24
     Column 2 = pin 23
     Column 3 = pin 22
   LCD 16x4 :
     1  (VSS)  = -
     2  (VDD)  = 5V
     3  (V0)   = GND
     4  (RS)   = pin 12
     5  (RW)   = GND
     6  (E)    = pin 11
     7  (DB0)  = -
     8  (DB1)  = -
     9  (DB2)  = -
     10 (DB3)  = -
     11 (DB4)  = pin 5
     12 (DB5)  = pin 4
     13 (DB6)  = pin 3
     14 (DB7)  = pin 2
     15 (LEDA) = 3.3V
     16 (LEDK) = GND
   Tiny RTC I2C Modules :
     SQ  = -
     DS  = -
     SCL = pin 21 (SCL)
     SDA = pin 20 (SDA)
     VCC = 5V
     GND = GND
   SD Card Module :
     GND  = -
     3V3  = -
     5V   = 5V
     CS   = pin 53
     MOSI = pin 51 (MOSI)
     SCK  = pin 52 (SCK)
     MISO = pin 50 (MISO)
     GND  = GND
   Receiver 433 MHz :
     DATA1 = pin 8
     DATA2 = -
     VCC   = 5V
     GND   = GND
   Indicator LED :
     Rx_Green  = pin 35
     RTC_Green = pin 37
     RTC_Red   = pin 39
     SD_Green  = pin 41
     SD_Yellow = pin 43
     SD_Red    = pin 45
> Thanks to :
   1. Alexander Brevig (alexanderbrevig@gmail.com) for providing :
       - CustomKeypad source code example
   2. David A. Mellis (Arduino Team) for providing :
       - LiquidCrystal source code example
       - SD Card (ReadWrite) code example
       - SD Card (Files code) example
   3. Tom Igoe (Arduino Team) for providing :
       - LiquidCrystal source code example
       - SD Card (Modified) code example
   4. Limo Fried for providing :
       - LiquidCrystal source code example
       - CardInfo code example
   5. Michael Margolis for providing :
       - SetTime example code
       - ReadTest example code
  6. Mohannad Rawashdeh (http://www.genotronex.com/) for providing :
	   - Simple Rx source code example
=============================================================================
                           Changelog
-----------------------------------------------------------------------------
v1.0  -
v1.1  1. Add a RTC module.
      2. Change the display of LCD. The display include:
         + Title and counter (in seconds)
         + Date [DD-MM-YYYY]
         + Clock [HH:MM:SS]
         + Keypress (for checking the keypad module)
v1.2  1. Add a SD Card module.
      2. Edit SETUP and MAIN LOOP.
      3. Add some procedure that can:
	     a. Save text in 'Ketik' label on LCD 16x4 to file every 2 second.
           File name: YYYYMMDD.TXT
         b. Delete saved file every year.
            Example: File 20140909.TXT is deleted when file 20150909.TXT
		             will be writed.
            So, every file has only duration one year.
v1.3  1. Create a title in file.
      2. Change the content on LCD 16x4
      3.
      4.

****************************************************************************/

/*------------------------( Import needed libraries )-----------------------------*/
#include <LiquidCrystal.h> // include the LCD library code
#include <Keypad.h> // include the 4x4 matrix keypad library code
#include <Wire.h>
#include <Time.h> // include time library code
#include <DS1307RTC.h> // include the DS1307 RTC library code
#include <SPI.h>
#include <SD.h> // include the SD library
#include <VirtualWire.h>

/*---------------------------------------------------------------------------------*/

/*---------------------( Declare Constants and Pin Numbers )-----------------------*/
/* LCD 16x4 */
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// status
const int row_SystemStatus = 1; // row that used to display status of system on LCD 16x4
const int column_SystemStatus_title = 0; // column that used to display status of system title on LCD 16x4
const int column_SystemStatus_content = column_SystemStatus_title + 8;
// keypress
const int first_keypress = 6; // first column for keypress cursor
const int row_keypress = 1; // row that used for keypress
const int length_of_LCD_keypress_value = 16 - first_keypress; // length of LCD keypress value
// date & clock
const int first_date_cursor = 0; // first column for date cursor
const int first_clock_cursor = first_date_cursor + 11; // first column for clock cursor
const int row_date = 0; // row that used to display the date
const int row_clock = row_date; // row that used to display the clock
// sensor data
const int column_Sensor_Temperature_title = 0; // column that used to display temperature title
const int column_Sensor_pH_title = 0; // column that used to display pH title
const int column_Sensor_DO_title = 0; // column that used to display DO title
const int column_Sensor_Turbidity_title = 0; // column that used to display turbidity title
const int column_Sensor_Salinity_title = 0; // column that used to display salinity title
const int column_Sensor_Depth_title = 0; // column that used to display depth title
const int column_Sensor_Temperature_value = column_Sensor_Temperature_title + 8; // column that used to display temperature value
const int column_Sensor_pH_value = column_Sensor_pH_title + 8; // column that used to display pH value
const int column_Sensor_DO_value = column_Sensor_DO_title + 8; // column that used to display DO value
const int column_Sensor_Turbidity_value = column_Sensor_Turbidity_title + 8; // column that used to display turbidity value
const int column_Sensor_Salinity_value = column_Sensor_Salinity_title + 8; // column that used to display salinity value
const int column_Sensor_Depth_value = column_Sensor_Depth_title + 8; // column that used to display depth value
const int row_Sensor_Temperature = 2; // row that used to display temperature value
const int row_Sensor_pH = 3; // row that used to display pH value
const int row_Sensor_DO = 2; // row that used to display DO value
const int row_Sensor_Turbidity = 3; // row that used to display turbidity value
const int row_Sensor_Salinity = 2; // row that used to display salinity value
const int row_Sensor_Depth = 3; // row that used to display depth value

/* RTC */
const int RTC_IndicatorLED_Green_PinNumber = 37; // pin number of indicator LED (Green) of RTC in Arduino
const int RTC_IndicatorLED_Red_PinNumber = 39; // pin number of indicator LED (Red) of RTC in Arduino

/* KEYPAD */
// constants for rows and columns on the keypad
const byte numRows = 4; // number of rows on the keypad
const byte numCols = 4; // number of columns on the keypad
// Keymap defines the key pressed according to
// the row and columns just as appears on the keypad.
char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {29, 28, 27, 26}; // rows 0 to 3
byte colPins[numCols] = {25, 24, 23, 22}; // columns 0 to 3
// initializes an instance of the keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

/* SD Card Module */
const int chipSelect = 53; // chip select
File myFile1, myFile2;
const int SDCard_IndicatorLED_Green_PinNumber = 41; // pin number of indicator LED (Green) of SD Card Module in Arduino
const int SDCard_IndicatorLED_Yellow_PinNumber = 43; // pin number of indicator LED (Yellow) of SD Card Module in Arduino
const int SDCard_IndicatorLED_Red_PinNumber = 45; // pin number of indicator LED (Red) of SD Card Module in Arduino

/* RECEIVER 433 MHz */
const int Rx_PinNumber = 8; // pin number of Rx in Arduino
const int Rx_IndicatorLED_Green_PinNumber = 35; // pin number of indicator LED (Green) of Rx in Arduino
const int Rx_SensorData_ALL_Length = 36; // 6 x 6 length of data
const int Rx_SensorData_Temperature_Length = 6; // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_pH_Length = 6;
const int Rx_SensorData_DO_Length = 6;
const int Rx_SensorData_Turbidity_Length = 6;
const int Rx_SensorData_Salinity_Length = 6;
const int Rx_SensorData_Depth_Length = 6;
/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare objects )---------------------------------*/
/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare Variables )-------------------------------*/
/* General */
int MainCounter;

/* LCD and KEYPAD */
char LCD_keypress_value[length_of_LCD_keypress_value]; // value of keypress
int LCD_keypress_value_index; // index of LCD_keypress_value
int LCD_i; // counter for keypress in line 2
int LCD_j; // dummy variable for display keypress counter in serial monitor
int LCD_SensorData_Display; // flag for changing display of sensor data

/* RTC */
tmElements_t tm;
int RTC_ThisDay; // for checking and removing old file
int RTC_NextDay; // for checking and removing old file

/* SD CARD MODULE */
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// file name for data logging
char SDCard_FileName[12]; // file name: YYYYMMDD.TXT
// for make sure only one data will be written
int SDCard_DataLogging_Write; // flag for writting process to SD card
// for marking the data that already received from RMP
int SDCard_Marker;

/* RECEIVER 433 MHz */
// variable for store sensor data from RMP module
/*char Rx_SensorData_ALL[Rx_SensorData_ALL_Length]; // store all data from RMP module
char Rx_SensorData_Temperature[Rx_SensorData_Temperature_Length]; // only store temperature value from RMP module
char Rx_SensorData_pH[Rx_SensorData_pH_Length]; // only store pH value from RMP module
char Rx_SensorData_DO[Rx_SensorData_DO_Length]; // only store DO value from RMP module
char Rx_SensorData_Turbidity[Rx_SensorData_Turbidity_Length]; // only store turbidity value from RMP module
char Rx_SensorData_Salinity[Rx_SensorData_Salinity_Length]; // only store salinity value from RMP module
char Rx_SensorData_Depth[Rx_SensorData_Depth_Length]; // only store the depths of the pond value from RMP module
*/
String Rx_SensorData_Temperature; // only store temperature value from RMP module
String Rx_SensorData_pH; // only store pH value from RMP module
String Rx_SensorData_DO; // only store DO value from RMP module
String Rx_SensorData_Turbidity; // only store turbidity value from RMP module
String Rx_SensorData_Salinity; // only store salinity value from RMP module
String Rx_SensorData_Depth; // only store the depths of the pond value from RMP module

/* etc. */
// value of converted string data (from RMP)
float Value_SensorData_Temperature;
float Value_SensorData_pH;
float Value_SensorData_DO;
float Value_SensorData_Turbidity;
float Value_SensorData_Salinity;
float Value_SensorData_Depth;
// status of Condition Check
String ConditionCheck_Status_Temperature;
String ConditionCheck_Status_pH;
String ConditionCheck_Status_DO;
String ConditionCheck_Status_Turbidity;
String ConditionCheck_Status_Salinity;
String ConditionCheck_Status_Depth;
/*---------------------------------------------------------------------------------*/

/*-----------------------------------( SETUP )-------------------------------------*/
void setup()
{
  /* Initialize the variables */
  MainCounter = 0; // initialization main counter
  procedure_Rx_SensorData_Initialization();
  procedure_Clear_LCD_keypress_value(); // reset or fill blank to LCD_keypress_value (replace with '_')
  LCD_keypress_value_index = 1;
  LCD_i = first_keypress;
  LCD_j = 1;
  RTC_ThisDay = tm.Day;
  RTC_NextDay = RTC_ThisDay + 1;
  SDCard_DataLogging_Write = 0; // flag for writting process to SD card
  LCD_SensorData_Display = 0; // flag for changing display of sensor data

  /* Serial Monitor */
  Serial.begin(9600);

  /* RECEIVER 433 MHz */
  procedure_Rx_SETUP();

  /* LCD 16x4 */
  procedure_LCD_SETUP_DateClock();
  procedure_LCD_SETUP_SystemStatus();
  procedure_LCD_SETUP_SensorData();
  //procedure_LCD_SETUP_KeypressLabel();

  /* RTC */
  procedure_RTC();

  /* SD CARD MODULE */
  procedure_SDCard_Initialization();
  procedure_SDCard_FileAvailableCheck();
}
/*---------------------------------------------------------------------------------*/

/*--------------------------------( MAIN LOOP )------------------------------------*/
void loop()
{
  // main counter
  procedure_Counter();

  // print date and clock to LCD
  procedure_RTC();

  /* REMOVE OLD FILE in SD CARD */
  RTC_ThisDay = tm.Day; // reset RTC_ThisDay variable with date of today
  if (RTC_ThisDay == RTC_NextDay)
  {
    procedure_SDCard_RemoveOldFile(); // remove file one year before
    procedure_SDCard_NewFile(); // create new file with a title in it
    RTC_NextDay += 1;
  }

  /* KEYPAD 4x4 */
  //procedure_LCD_Keypress();

  /* RTC */
  procedure_Rx_ReceivingDataRMP();
  procedure_SensorData_ConvertStringToFloat();
  procedure_SensorData_ConditionCheck();

  /* SENSOR DATA & LCD 16x4 */
  procedure_LCD_SensorData_Display();

  /* SD CARD MODULE */
  if ((SDCard_DataLogging_Write == 0) && (tm.Second % 2 != 0))
  {
    SDCard_DataLogging_Write = 1;
  }
  procedure_LED_SystemStatus_WriteFile_Clear(); // clear the status of writing file
  // writing data to SD card
  if ((SDCard_DataLogging_Write == 1) && (tm.Second % 2 == 0))
  {
    procedure_TEST_DisplayConvertStringToFloat(); // TEST
    procedure_SDCard_FileName();
    procedure_SDCard_DataLogging();
    SDCard_DataLogging_Write = 0;
  }
}
/*---------------------------------------------------------------------------------*/

/*-----------------------------( PROCEDURE DETAIL )--------------------------------*/
void procedure_Counter()
// input   : N/A
// output  : counter in integer
// process : counting every 1 second
{
  if (MainCounter < 3600)
  {
    MainCounter = millis() / 1000;
  } else {
    MainCounter = 0;
  }
}

void procedure_LED_SystemStatus_SETUP_All()
// input   : ...
// output  : ...
// process : ...
{
  /* SD CARD MODULE */
  pinMode(SDCard_IndicatorLED_Green_PinNumber, OUTPUT);
  pinMode(SDCard_IndicatorLED_Yellow_PinNumber, OUTPUT);
  pinMode(SDCard_IndicatorLED_Red_PinNumber, OUTPUT);
  digitalWrite(SDCard_IndicatorLED_Green_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 0);

  /* RECEIVER 433 MHz */
  pinMode(Rx_IndicatorLED_Green_PinNumber, OUTPUT);
  digitalWrite(Rx_IndicatorLED_Green_PinNumber, 0);

  /* RTC */
  pinMode(RTC_IndicatorLED_Green_PinNumber, OUTPUT);
  pinMode(RTC_IndicatorLED_Red_PinNumber, OUTPUT);
  digitalWrite(RTC_IndicatorLED_Green_PinNumber, 0);
  digitalWrite(RTC_IndicatorLED_Red_PinNumber, 0);
}

void procedure_LED_SystemStatus_WriteFile_Done()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(SDCard_IndicatorLED_Green_PinNumber, 1);
}

void procedure_LED_SystemStatus_WriteFile_Error()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 1);

  lcd.setCursor(column_SystemStatus_content, row_SystemStatus); // column = column_SystemStatus_content, row = row_SystemStatus
  lcd.print("writeERR");
}

void procedure_LED_SystemStatus_WriteFile_Clear()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(SDCard_IndicatorLED_Green_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 0);
}

void procedure_LED_SystemStatus_DeleteFile_Done()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 1);
}

void procedure_LED_SystemStatus_DeleteFile_Error()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 1);

  lcd.setCursor(column_SystemStatus_content, row_SystemStatus); // column = column_SystemStatus_content, row = row_SystemStatus
  lcd.print("del_ERR");
}

void procedure_LED_SystemStatus_RTC_Worked()
// input   : ...
// output  : ...
// process : ...
{
  digitalWrite(RTC_IndicatorLED_Green_PinNumber, 1);
}

void procedure_LED_SystemStatus_RTC_Error()
// input   : ...
// output  : ...
// process : ...
{
  if (RTC.chipPresent()) {
    digitalWrite(RTC_IndicatorLED_Red_PinNumber, 1);
    lcd.setCursor(column_SystemStatus_content, row_SystemStatus); // column = column_SystemStatus_content, row = row_SystemStatus
    lcd.print("RTC_ERR1");
  } else {
    digitalWrite(RTC_IndicatorLED_Red_PinNumber, 1);
    lcd.setCursor(column_SystemStatus_content, row_SystemStatus); // column = column_SystemStatus_content, row = row_SystemStatus
    lcd.print("RTC_ERR2");
  }
}

void procedure_Rx_SETUP()
// input   : ...
// output  : ...
// process : ...
{
  vw_set_ptt_inverted(true); // required for DR3100
  vw_set_rx_pin(Rx_PinNumber);
  vw_setup(4000); // bits per sec
  vw_rx_start(); // start the receiver PLL running
}

void procedure_Rx_SensorData_Initialization()
// input   : ...
// output  : ...
// process : ...
{
  Rx_SensorData_Temperature = "     ";
  Rx_SensorData_pH = "     ";
  Rx_SensorData_DO = "     ";
  Rx_SensorData_Turbidity = "     ";
  Rx_SensorData_Salinity = "     ";
  Rx_SensorData_Depth = "     ";
}

void procedure_Rx_ReceivingDataRMP()
// input   : ...
// output  : ...
// process : ...
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  /*
    uint8_t buf[30];
    uint8_t buflen = 30;
  */
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(Rx_IndicatorLED_Green_PinNumber, 1);
    SDCard_Marker = 1;

    // sensor data of TEMPERATURE
    Serial.print("Temp  ="); // serial monitor (Arduino IDE)
    for (int i = 0; i <= 4; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Temperature[i] = ' ';
      Rx_SensorData_Temperature[i] = buf[i];
    }
    Serial.println();

    // sensor data of PH
    Serial.print("pH    ="); // serial monitor (Arduino IDE)
    for (int i = 5; i <= 9; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_pH[i - 5] = ' ';
      Rx_SensorData_pH[i - 5] = buf[i];
    }
    Serial.println();

    // sensor data of DO
    Serial.print("DO    ="); // serial monitor (Arduino IDE)
    for (int i = 10; i <= 14; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_DO[i - 10] = ' ';
      Rx_SensorData_DO[i - 10] = buf[i];
    }
    Serial.println();

    // sensor data of TURBIDITY
    Serial.print("Keruh ="); // serial monitor (Arduino IDE)
    for (int i = 15; i <= 19; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Turbidity[i - 15] = ' ';
      Rx_SensorData_Turbidity[i - 15] = buf[i];
    }
    Serial.println();

    // sensor data of SALINITY
    Serial.print("Garam ="); // serial monitor (Arduino IDE)
    for (int i = 20; i <= 24; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Salinity[i - 20] = ' ';
      Rx_SensorData_Salinity[i - 20] = buf[i];
    }
    Serial.println();

    // sensor data of DEPTH
    Serial.print("Dalam ="); // serial monitor (Arduino IDE)
    for (int i = 25; i <= 29; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Depth[i - 25] = ' ';
      Rx_SensorData_Depth[i - 25] = buf[i];
    }
    Serial.println();

    digitalWrite(Rx_IndicatorLED_Green_PinNumber, 0);
  }
  //  Serial.println("tidak ada koneksi");
}

void procedure_LCD_SETUP_DateClock()
// input   : ...
// output  : ...
// process : ...
{
  // set up the LCD's number of columns and rows
  lcd.begin(16, 4); // sum of column = 16, sum of row = 4

  // date
  lcd.setCursor(first_date_cursor + 2, row_date); // column = first_date_cursor + 2, row = row_date
  lcd.print("-");
  lcd.setCursor(first_date_cursor + 5, row_date); // column = first_date_cursor + 5, row = row_date
  lcd.print("-");

  // clock
  lcd.setCursor(first_clock_cursor + 2, row_clock); // column = first_clock_cursor + 2, row = row_date
  lcd.print(":");
}

void procedure_LCD_SETUP_SystemStatus()
// input   : ...
// output  : ...
// process : ...
{
  lcd.setCursor(column_SystemStatus_title, row_SystemStatus);
  lcd.print("Status:");
}

void procedure_LCD_SETUP_SensorData()
// input   : ...
// output  : ...
// process : ...
{
  // temperature
  lcd.setCursor(column_Sensor_Temperature_title, row_Sensor_Temperature); // column = column_Sensor_Temperature_title, row = row_Sensor_Temperature
  lcd.print("Suhu  =");

  // pH
  lcd.setCursor(column_Sensor_pH_title, row_Sensor_pH); // column = column_Sensor_pH_title, row = row_Sensor_pH
  lcd.print("pH    =");
}

void procedure_LCD_SETUP_KeypressLabel() // temporarly NOT USED because DATA SENSOR testing
// input   : ...
// output  : ...
// process : ...
// NOTE	   : DO NOT DELETE this procedure!
//           store this procedure in SETUP section
{
  // print label to the LCD
  // keypress label
  lcd.setCursor(0, row_keypress); // column 0, row = row_keypress
  lcd.print("Ketik");
  lcd.setCursor(first_keypress, row_keypress); // column = first_keypress, row = row_keypress
  procedure_ClearScreen_keypress(); // clear the screen of keypress label
}

void procedure_LCD_Keypress() // temporarly NOT USED because DATA SENSOR testing
// input   : ...
// output  : ...
// process : ...
// NOTE	   : DO NOT DELETE this procedure!
//           store this procedure in MAIN LOOP section
{
  char keypressed = myKeypad.getKey();

  /* KEYPAD */
  // If key is pressed, this key is stored in 'keypressed' variable.
  // If key is not equal to 'NO_KEY', then this key is printed out.
  // If count = 17, then count is reset back to 0
  // (this means no key is pressed during the whole keypad scan process).
  if ((keypressed != NO_KEY) && (LCD_i <= 15))
  {
    // display the keypress on LCD
    lcd.setCursor(LCD_i, row_keypress); // column = LCD_i, row = row_keypress
    LCD_keypress_value[LCD_keypress_value_index] = keypressed; // fill LCD_keypress_value with keypressed
    lcd.print(LCD_keypress_value[LCD_keypress_value_index]); // print LCD_keypress_value
    LCD_keypress_value_index++;
    LCD_i++;
    LCD_j++;
  }
  else if ((keypressed != NO_KEY) && (LCD_i > 15))
  {
    // reset some variable
    LCD_i = first_keypress; // reset LCD_i back to first_keypress
    LCD_keypress_value_index = 1; // reset the index back to 1
    procedure_Clear_LCD_keypress_value(); // reset or fill blank to LCD_keypress_value (replace with '_')

    // clear-screen the keypress label on LCD
    lcd.setCursor(LCD_i, row_keypress); // column LCD_i, row = row_keypress
    procedure_ClearScreen_keypress(); // clear the screen of keypress label

    // display the keypress on LCD (same with before)
    lcd.setCursor(LCD_i, row_keypress); // column = LCD_i, row = row_keypress
    LCD_keypress_value[LCD_keypress_value_index] = keypressed; // fill LCD_keypress_value with keypressed
    lcd.print(LCD_keypress_value[LCD_keypress_value_index]); // print LCD_keypress_value
    LCD_keypress_value_index++;
    LCD_i++;
    LCD_j++;
  }
}

void procedure_LCD_SensorData_Display()
// input   : ...
// output  : ...
// process : ...
{
  if ((LCD_SensorData_Display == 0) && (MainCounter % 6 == 0))
  {
    // temperature
    lcd.setCursor(column_Sensor_Temperature_title, row_Sensor_Temperature); // column = column_Sensor_Temperature_title, row = row_Sensor_Temperature
    lcd.print("Suhu  =");
    lcd.setCursor(column_Sensor_Temperature_value, row_Sensor_Temperature); // column = column_Sensor_Temperature_value, row = row_Sensor_Temperature
    lcd.print(Rx_SensorData_Temperature);
    lcd.setCursor(column_Sensor_Temperature_value + 6, row_Sensor_Temperature); // column = column_Sensor_Temperature_value + 7, row = row_Sensor_Temperature
    lcd.print(ConditionCheck_Status_Temperature);

    // pH
    lcd.setCursor(column_Sensor_pH_title, row_Sensor_pH); // column = column_Sensor_pH_title, row = row_Sensor_pH
    lcd.print("pH    =");
    lcd.setCursor(column_Sensor_pH_value, row_Sensor_pH); // column = column_Sensor_pH_value, row = row_Sensor_pH
    lcd.print(Rx_SensorData_pH);
    lcd.setCursor(column_Sensor_pH_value + 6, row_Sensor_pH); // column = column_Sensor_pH_value + 7, row = row_Sensor_pH
    lcd.print(ConditionCheck_Status_pH);

    LCD_SensorData_Display ++;
  }
  // display other data sensor
  if ((LCD_SensorData_Display == 1) && (MainCounter % 6 == 2))
  {
    // DO
    lcd.setCursor(column_Sensor_DO_title, row_Sensor_DO); // column = column_Sensor_DO_title, row = row_Sensor_DO
    lcd.print("DO    =");
    lcd.setCursor(column_Sensor_DO_value, row_Sensor_DO); // column = column_Sensor_DO_value, row = row_Sensor_DO
    lcd.print(Rx_SensorData_DO);
    lcd.setCursor(column_Sensor_DO_value + 6, row_Sensor_DO); // column = column_Sensor_DO_value + 7, row = row_Sensor_DO
    lcd.print(ConditionCheck_Status_DO);
    // turbidity
    lcd.setCursor(column_Sensor_Turbidity_title, row_Sensor_Turbidity); // column = column_Sensor_Turbidity_title, row = row_Sensor_Turbidity
    lcd.print("Keruh =");
    lcd.setCursor(column_Sensor_Turbidity_value, row_Sensor_Turbidity); // column = column_Sensor_Turbidity_value, row = row_Sensor_Turbidity
    lcd.print(Rx_SensorData_Turbidity);
    lcd.setCursor(column_Sensor_Turbidity_value + 6, row_Sensor_Turbidity); // column = column_Sensor_Turbidity_value + 7, row = row_Sensor_Turbidity
    lcd.print(ConditionCheck_Status_Turbidity);

    LCD_SensorData_Display++;
  }
  // display other data sensor
  if ((LCD_SensorData_Display == 2) && (MainCounter % 6 == 4))
  {
    // salinity
    lcd.setCursor(column_Sensor_Salinity_title, row_Sensor_Salinity); // column = column_Sensor_Salinity_title, row = row_Sensor_Salinity
    lcd.print("Garam =");
    lcd.setCursor(column_Sensor_Salinity_value, row_Sensor_Salinity); // column = column_Sensor_Salinity_value, row = row_Sensor_Salinity
    lcd.print(Rx_SensorData_Salinity);
    lcd.setCursor(column_Sensor_Salinity_value + 6, row_Sensor_Salinity); // column = column_Sensor_Salinity_value + 7, row = row_Sensor_Salinity
    lcd.print(ConditionCheck_Status_Salinity);
    // depth
    lcd.setCursor(column_Sensor_Depth_title, row_Sensor_Depth); // column = column_Sensor_Depth_title, row = row_Sensor_Depth
    lcd.print("Dalam =");
    lcd.setCursor(column_Sensor_Depth_value, row_Sensor_Depth); // column = column_Sensor_Depth_value, row = row_Sensor_Depth
    lcd.print(Rx_SensorData_Depth);
    lcd.setCursor(column_Sensor_Depth_value + 6, row_Sensor_Depth); // column = column_Sensor_Depth_value + 7, row = row_Sensor_Depth
    lcd.print(ConditionCheck_Status_Depth);

    LCD_SensorData_Display = 0 ;
  }
}

void procedure_Clear_LCD_keypress_value()
// input   : N/A
// output  : LCD_keypress_value variable is filled with '_'
// process : replace LCD_keypress_value variable with '_'
{
  for (int i = 1; i <= length_of_LCD_keypress_value; i++)
  {
    LCD_keypress_value[i] = '_';
  }
}

void procedure_ClearScreen_keypress()
// input   : N/A
// output  : blank on LCD (keypress label)
// process : replace text on LCD (keypress label) with '_'
{
  char blank[length_of_LCD_keypress_value];

  for (int i = 1; i <= length_of_LCD_keypress_value; i++)
  {
    blank[i] = '_';
  }
  for (int i = 1; i <= length_of_LCD_keypress_value; i++)
  {
    lcd.print(blank[i]);
  }
}

void procedure_RTC()
// input   : N/A
// output  : display of clock and date
// process : check clock and date from RTC module
{
  if (RTC.read(tm))
  {
    /* LED for RTC Status */
    procedure_LED_SystemStatus_RTC_Worked();

    /* Print DATE to LCD */
    // Day [1..31]
    if (tm.Day < 10)
    {
      lcd.setCursor(first_date_cursor, row_date); // column = first_date_cursor, row = row_date
      lcd.print('0'); // Day [1..31]
      lcd.setCursor(first_date_cursor + 1 , row_date); // column = first_date_cursor + 1, row = row_date
      lcd.print(tm.Day); // Day [1..31]
    } else
    {
      lcd.setCursor(first_date_cursor, row_date); // column = first_date_cursor, row = row_date
      lcd.print(tm.Day); // Day [1..31]
    }

    // Month [1..12]
    if (tm.Month < 10)
    {
      lcd.setCursor(first_date_cursor + 3, row_date); // column = first_date_cursor + 3, row = row_date
      lcd.print('0'); // Month [1..12]
      lcd.setCursor(first_date_cursor + 4, row_date); // column = first_date_cursor + 4, row = row_date
      lcd.print(tm.Month); // Month [1..12]
    } else
    {
      lcd.setCursor(first_date_cursor + 3, row_date); // column = first_date_cursor + 3, row = row_date
      lcd.print(tm.Month); // Month [1..12]
    }

    // Year [xxxx]
    lcd.setCursor(first_date_cursor + 6, row_date); // column = first_date_cursor + 6, row = row_date
    lcd.print(tmYearToCalendar(tm.Year)); // Year [xxxx]

    /* Print CLOCK to LCD */
    // Hour [0..23]
    if (tm.Hour < 10) // Hour of RTC < 10
    {
      lcd.setCursor(first_clock_cursor, row_clock); // column = first_clock_cursor, row = row_clock
      lcd.print('0'); // Hour [0..23]
      lcd.setCursor(first_clock_cursor + 1, row_clock); // column = first_clock_cursor + 1, row = row_clock
      lcd.print(tm.Hour); // Hour [0..23]
    } else // Hour of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor, row_clock); // column = first_clock_cursor, row = row_clock
      lcd.print(tm.Hour); // Hour [0..23]
    }

    // Minute [0..59]
    if (tm.Minute < 10) // Minute of RTC < 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock); // column = first_clock_cursor + 3, row 3 = row_clock
      lcd.print('0'); // Minute [0..59]
      lcd.setCursor(first_clock_cursor + 4, row_clock); // column = first_clock_cursor + 4, row 3 = row_clock
      lcd.print(tm.Minute); // Minute [0..59]
    } else // Minute of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock); // column = first_clock_cursor + 3, row = row_clock
      lcd.print(tm.Minute); // Minute [0..59]
    }
  } else {
    procedure_LED_SystemStatus_RTC_Error();
  }
}

void procedure_RTC_SecondSeparator() // NOT YET used, still DEVELOPING
// input   : ...
// output  : ...
// process : ...
{
  /*
    lcd.setCursor(first_clock_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
    lcd.print(" ");
    delay(500);
    lcd.setCursor(first_clock_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
    lcd.print(":");
    delay(500);
  */

  /*
  int CountingSecond;

  CountingSecond = millis();

  if (CountingSecond % 1000 == 500)
  {
    lcd.setCursor(first_clock_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print(CountingSecond);
  } else
  {
    lcd.setCursor(first_clock_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
    lcd.print(":");
    lcd.setCursor(0, 3);
    lcd.print(CountingSecond);
  }
  */
}

void procedure_SDCard_Initialization()
// input   : N/A
// output  : status of SD card
// process : check the present of SD card
{
  /* initializing #1 */
  /*  Serial.println("#1");
    Serial.print("Initializing SD card...");

    // in first initialization, we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
      Serial.println("initialization failed. Things to check:");
      Serial.println("* is a card inserted?");
      Serial.println("* is your wiring correct?");
      Serial.println("* did you change the chipSelect pin to match your shield or module?");
      return;
    } else {
      Serial.println("Wiring is correct and a card is present.");
    }
  */
  /* Initializing #2 */
  Serial.println("#2");
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

}

void procedure_SDCard_FileName()
// input   : ...
// output  : ...
// process : ...
{
  // local variable
  int YYYY;
  int MM;
  int DD;

  /* Create File Name */
  // initializing
  YYYY = tmYearToCalendar(tm.Year); // Year
  MM = tm.Month; // Month
  DD = tm.Day; // Day

  if ((MM < 10) && (DD < 10)) { // Month < 10 AND Day < 10
    sprintf(SDCard_FileName, "%d0%d0%d.TXT", YYYY, MM, DD);
  }
  else if ((MM < 10) && (DD >= 10)) { // Month < 10 AND Day >= 10
    sprintf(SDCard_FileName, "%d0%d%d.TXT", YYYY, MM, DD);
  }
  else if ((MM >= 10) && (DD < 10)) { // Month >= 10 AND Day < 10
    sprintf(SDCard_FileName, "%d%d0%d.TXT", YYYY, MM, DD);
  }
  else { // Month >= 10 AND Day >= 10
    sprintf(SDCard_FileName, "%d%d%d.TXT", YYYY, MM, DD);
  }
}

void procedure_SDCard_FileAvailableCheck()
// input   : ...
// output  : ...
// process : ...
{
  procedure_SDCard_FileName();
  if (!SD.exists(SDCard_FileName)) {
    procedure_SDCard_RemoveOldFile();
    procedure_SDCard_NewFile();
  }
}

void procedure_SDCard_RemoveOldFile()
// input   : N/A
// output  : old files removed
// process : removing old files
{
  // local variable
  int YYYY;
  int MM;
  int DD;
  char FileName[12];

  /* Delete File Name */
  // initializing
  YYYY = tmYearToCalendar(tm.Year) - 1; // Year
  MM = tm.Month; // Month
  DD = tm.Day; // Day

  // define file name
  if ((MM < 10) && (DD < 10)) { // Month < 10 AND Day < 10
    sprintf(FileName, "%d0%d0%d.TXT", YYYY, MM, DD);
  }
  else if ((MM < 10) && (DD >= 10)) { // Month < 10 AND Day >= 10
    sprintf(FileName, "%d0%d%d.TXT", YYYY, MM, DD);
  }
  else if ((MM >= 10) && (DD < 10)) { // Month >= 10 AND Day < 10
    sprintf(FileName, "%d%d0%d.TXT", YYYY, MM, DD);
  }
  else { // Month >= 10 AND Day >= 10
    sprintf(FileName, "%d%d%d.TXT", YYYY, MM, DD);
  }

  // check the file before removed
  if (SD.exists(FileName)) {
    Serial.print("File ");
    Serial.print(FileName);
    Serial.println(" must be deleted!");
    // remove the old file
    SD.remove(FileName);
    // check that the file has removed or not
    if (SD.exists(FileName)) {
      Serial.print("Deleting ");
      Serial.print(FileName);
      Serial.println(" FAILED!");
      procedure_LED_SystemStatus_DeleteFile_Error();
    }
    else {
      Serial.print("Deleting ");
      Serial.print(FileName);
      Serial.println(" COMPLETED!");
      procedure_LED_SystemStatus_DeleteFile_Done();
    }
  }
  else {
    Serial.println("No file need to be deleted!");
  }
}

void procedure_SDCard_NewFile()
// input   : ...
// output  : ...
// process : ...
{
  /* NEW FILE for SENSOR DATA LOGGING - YYYYMMDD.TXT */
  // open the file. note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open YYYYMMDD.TXT file that already exist
  // if it isn't, then create YYYYMMDD.TXT as new file
  myFile1 = SD.open(SDCard_FileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1) {
    Serial.print("Writing to "); // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.print("...");

    /* title */
    myFile1.println("YYYYMMDD_hhmmss_Suhu-_pH---_DO---_Turb-_Salnt_Depth");

    // close the file
    myFile1.close();

    Serial.println("done."); // display on serial monitor (Arduino IDE)

  } else {
    // if the file didn't open, print an error
    Serial.print("[NewFile] ERROR opening "); // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.println("!");
  }
}

void procedure_SDCard_DataLogging()
// input   : ...
// output  : ...
// process : ...
{
  /* SENSOR DATA LOGGING - YYYYMMDD.TXT */
  // open the file. note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open YYYYMMDD.TXT file that already exist
  // if it isn't, then create YYYYMMDD.TXT as new file
  myFile1 = SD.open(SDCard_FileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1) {
    Serial.print("Writing to "); // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.print("...");

    /* date */
    // Year
    myFile1.print(tmYearToCalendar(tm.Year));
    // Month
    if (tm.Month < 10) { // month < 10
      myFile1.print('0');
      myFile1.print(tm.Month);
    } else { // month >= 10
      myFile1.print(tm.Month);
    }
    // Day
    if (tm.Day < 10) { // day < 10
      myFile1.print('0');
      myFile1.print(tm.Day);
    } else { // day >= 10
      myFile1.print(tm.Day);
    }

    /* separator */
    myFile1.print('_');

    /* clock */
    // Hour
    if (tm.Hour < 10) { // Hour < 10
      myFile1.print('0');
      myFile1.print(tm.Hour);
    } else { // Hour >= 10
      myFile1.print(tm.Hour);
    }
    // Minute
    if (tm.Minute < 10) { // Minute < 10
      myFile1.print('0');
      myFile1.print(tm.Minute);
    } else { // Minute >= 10
      myFile1.print(tm.Minute);
    }
    // Second
    if (tm.Second < 10) { // Second < 10
      myFile1.print('0');
      myFile1.print(tm.Second);
    } else { // Second >= 10
      myFile1.print(tm.Second);
    }

    /* separator */
    myFile1.print('_');

    //    /* keypress */
    //    for (int i = 1; i <= length_of_LCD_keypress_value; i++)
    //    {
    //      myFile1.print(LCD_keypress_value[i]);
    //    }

    /* sensor data */
    myFile1.print(Rx_SensorData_Temperature);
    myFile1.print('_');
    myFile1.print(Rx_SensorData_pH);
    myFile1.print('_');
    myFile1.print(Rx_SensorData_DO);
    myFile1.print('_');
    myFile1.print(Rx_SensorData_Turbidity);
    myFile1.print('_');
    myFile1.print(Rx_SensorData_Salinity);
    myFile1.print('_');
    myFile1.print(Rx_SensorData_Depth);
    myFile1.print('_');
    myFile1.print(SDCard_Marker);
    myFile1.println("");

    // close the file
    myFile1.close();

    Serial.println("done."); // display on serial monitor (Arduino IDE)

    // display status on LCD 16x4
    procedure_LED_SystemStatus_WriteFile_Done();

    // reset SDCard_Marker
    SDCard_Marker = 0;

  } else {
    // if the file didn't open, print an error
    Serial.print("[DataLogging] ERROR opening "); // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.println("!");

    // display status on LCD 16x4
    procedure_LED_SystemStatus_WriteFile_Error();
  }
}

void procedure_SensorData_ConvertStringToFloat()
// input   : ...
// output  : ...
// process : ...
{
  Value_SensorData_Temperature = Rx_SensorData_Temperature.toFloat();
  Value_SensorData_pH = Rx_SensorData_pH.toFloat();
  Value_SensorData_DO = Rx_SensorData_DO.toFloat();
  Value_SensorData_Turbidity = Rx_SensorData_Turbidity.toFloat();
  Value_SensorData_Salinity = Rx_SensorData_Salinity.toFloat();
  Value_SensorData_Depth = Rx_SensorData_Depth.toFloat();
}

void procedure_SensorData_ConditionCheck()
// input   : ...
// output  : ...
// process : ...
{
  /* Standard Value Based on Standar Nasional Indonesia (SNI) */
  // temperature (Celcius degree)
  float Temperature_Low = 28.5;
  float Temperature_High = 31.5;

  // pH
  float pH_Low = 7.5;
  float pH_High = 8.5;

  // DO (miligram per liter)
  float DO_Low = 3.5;

  // turbidity


  // salinity (gram per liter)
  float Salinity_Low = 15;
  float Salinity_High = 25;

  // depth (centimeter)
  float Depth_Low = 120;
  float Depth_High = 200;

  /* Condition Check */
  // sensor data of TEMPERATURE
  if ((Value_SensorData_Temperature >= Temperature_Low) && (Value_SensorData_Temperature <= Temperature_High))
  {
    ConditionCheck_Status_Temperature = "OK";
  } else
  {
    ConditionCheck_Status_Temperature = "xx";
  }

  // sensor data of PH
  if ((Value_SensorData_pH >= pH_Low) && (Value_SensorData_pH <= pH_High))
  {
    ConditionCheck_Status_pH = "OK";
  } else
  {
    ConditionCheck_Status_pH = "xx";
  }

  // sensor data of DO
  if (Value_SensorData_DO >= DO_Low)
  {
    ConditionCheck_Status_DO = "OK";
  } else
  {
    ConditionCheck_Status_DO = "xx";
  }

  // sensor data of TURBIDITY


  // sensor data of SALINITY
  if ((Value_SensorData_Salinity >= Salinity_Low) && (Value_SensorData_Salinity <= Salinity_High))
  {
    ConditionCheck_Status_Salinity = "OK";
  } else
  {
    ConditionCheck_Status_Salinity = "xx";
  }

  // sensor data of DEPTH
  if ((Value_SensorData_Depth >= Depth_Low) && (Value_SensorData_Depth <= Depth_High))
  {
    ConditionCheck_Status_Depth = "OK";
  } else
  {
    ConditionCheck_Status_Depth = "xx";
  }
}

void procedure_TEST_DisplayConvertStringToFloat()
// input   : ...
// output  : ...
// process : ...
{
  Serial.print("Temperature = ");
  Serial.println(Value_SensorData_Temperature);
  Serial.print("pH = ");
  Serial.println(Value_SensorData_pH);
  Serial.print("DO = ");
  Serial.println(Value_SensorData_DO);
  Serial.print("Turbidity = ");
  Serial.println(Value_SensorData_Turbidity);
  Serial.print("Salinity = ");
  Serial.println(Value_SensorData_Salinity);
  Serial.print("Depth = ");
  Serial.println(Value_SensorData_Depth);
}

void procedure_SDCard_TEST_all() // for TESTING only - not combined with others
// input   : ...
// output  : ...
// process : ...
// NOTE	   : DO NOT DELETE this procedure!
//			 This procedure is used for testing the function of SD Card
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // initializing #1 ----------------------------------------------------------------------
  Serial.print("#1");
  Serial.print("\nInitializing SD card...");

  // in first initialization, we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  // --------------------------------------------------------------------------------------

  // initializing #2 ----------------------------------------------------------------------
  Serial.print("\n#2");
  Serial.print("\nInitializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  // --------------------------------------------------------------------------------------

  // print the type of card ---------------------------------------------------------------
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
  // --------------------------------------------------------------------------------------

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32 -------
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition");
    Serial.println("Make sure you've formatted the card");
    return;
  }
  // --------------------------------------------------------------------------------------

  // print the type and size of the first FAT-type volume ---------------------------------
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();  // clusters are collections of blocks
  volumesize *= volume.clusterCount();     // we'll have a lot of clusters
  volumesize *= 512;                       // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  // --------------------------------------------------------------------------------------

  // list all files in the card with date and size ----------------------------------------
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE); // list name, date, and size
  // --------------------------------------------------------------------------------------


  // FILE 1 - test.txt --------------------------------------------------------------------
  // open the file. note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open test.txt file that already exist
  // if it isn't, then create test.txt as new file
  myFile1 = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1) {
    Serial.print("\nWriting to test.txt...");
    myFile1.println("FILE 1 - test.txt");
    myFile1.println("testing 1, 2, 3, 4, 5.");
    // close the file:
    myFile1.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt\n");
  }

  // re-open the file for reading:
  Serial.print("\nReading test.txt after writing...\n");
  myFile1 = SD.open("test.txt");
  if (myFile1) {
    Serial.println("\nText in test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile1.available()) {
      Serial.write(myFile1.read());
    }
    // close the file:
    myFile1.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt\n");
  }
  // --------------------------------------------------------------------------------------

  // FILE 2 - test.txt --------------------------------------------------------------------
  // open the file. note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open test2.txt file that already exist
  // if it isn't, then create test2.txt as new file
  myFile2 = SD.open("test2.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile2) {
    Serial.print("\nWriting to test2.txt...");
    myFile2.println("FILE 2 - test2.txt");
    myFile2.println("testing 1, 2, 3, 4, 5.");
    // close the file:
    myFile2.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test2.txt\n");
  }

  // re-open the file for reading:
  Serial.print("\nReading test2.txt after writing...\n");
  myFile2 = SD.open("test2.txt");
  if (myFile2) {
    Serial.println("\nText in test2.txt:");

    // read from the file until there's nothing else in it:
    while (myFile2.available()) {
      Serial.write(myFile2.read());
    }
    // close the file:
    myFile2.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test2.txt\n");
  }
  // ---------------------------------------------------------------------------------------

  // list all files in the card with date and size, after writing -------------------------
  Serial.println("\nAfter writing files...");
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE); // list name, date, and size
  // --------------------------------------------------------------------------------------

  // DELETE: FILE 1 test.txt --------------------------------------------------------------
  // delete the file:
  Serial.println("\nRemoving test.txt...");
  SD.remove("test.txt");

  if (SD.exists("test.txt")) {
    Serial.println("deleting test.txt FAILED!");
  }
  else {
    Serial.println("deleting test.txt COMPLETED!");
  }
  // --------------------------------------------------------------------------------------

  // list all files in the card with date and size, after deleting ------------------------
  Serial.println("\nAfter deleting files...");
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE); // list name, date, and size
  // --------------------------------------------------------------------------------------
}
/*---------------------------------------------------------------------------------*/
