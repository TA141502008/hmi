/*
 * Project Name   : Experiment
 *                  Prototipe of Final Project: Human Machine Interface (HMI)
 * Revision       : 0.6
 * Date Created   : August 27, 2015
 * Date Revised   : February 14, 2016
 * Author         : Baharuddin Aziz
 * Author Mail    : project@baha.web.id
 * Company        : Department of Electrical Engineering
 *                  School of Electrical Engineering and Informatics
 *                  Bandung Institute of Technology (ITB)
 * Summary        : 1. Initialize all component.
 *                  2. Do counting every second, reset every 3600 seconds.
 *                  3. Display date and clock on LCD 16x4.
 *                  4. Remove old file in SD card, then create file which name
 *                     is YYYYMMDD.TXT of the day.
 *                  5. Receive sensor data from RMP module.
 *                  6. Turn ON alarm when 1 of sensor parameter is NOT normal.
 *                  7. Display the sensor data on LCD 16x4.
 *                  8. Save the sensor data in SD card.
 */

/*=======================( IMPORT NEEDED LIBRARY )===========================*/
#include <LiquidCrystal.h>  // include the LCD library code
#include <Keypad.h>         // include the 4x4 matrix keypad library code
#include <Wire.h>
#include <Time.h>           // include time library code
#include <DS1307RTC.h>      // include the DS1307 RTC library code
#include <SPI.h>
#include <SD.h>             // include the SD library
#include <VirtualWire.h>
// GSM Module
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
/*===========================================================================*/


/*==================( DECLARE CONSTANTS and PIN NUMBER )=====================*/
/* LCD 16x4 */
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // pin number that used to interface LCD 16x4
// status
const int row_SystemStatus = 1;                                         // row that used to display status of system
const int column_SystemStatus_title = 0;                                // column that used to display status of system (title)
const int column_SystemStatus_content = column_SystemStatus_title + 8;  // column that used to display content of status of system
// date & clock
const int row_date = 0;          // row that used to display the date
const int row_clock = row_date;  // row that used to display the clock
const int first_date_cursor = 0;                        // first column for date cursor
const int first_clock_cursor = first_date_cursor + 11;  // first column for clock cursor
// sensor data
const int row_Sensor_Temperature = 2;  // row that used to display temperature value
const int row_Sensor_pH = 3;           // row that used to display pH value
const int row_Sensor_DO = 2;           // row that used to display DO value
const int row_Sensor_Turbidity = 3;    // row that used to display turbidity value
const int row_Sensor_Salinity = 2;     // row that used to display salinity value
const int row_Sensor_Depth = 3;        // row that used to display depth value
const int column_Sensor_Temperature_title = 0;  // column that used to display temperature title
const int column_Sensor_pH_title = 0;           // column that used to display pH title
const int column_Sensor_DO_title = 0;           // column that used to display DO title
const int column_Sensor_Turbidity_title = 0;    // column that used to display turbidity title
const int column_Sensor_Salinity_title = 0;     // column that used to display salinity title
const int column_Sensor_Depth_title = 0;        // column that used to display depth title
const int column_Sensor_Temperature_value = column_Sensor_Temperature_title + 8;  // column that used to display temperature value
const int column_Sensor_pH_value = column_Sensor_pH_title + 8;                    // column that used to display pH value
const int column_Sensor_DO_value = column_Sensor_DO_title + 8;                    // column that used to display DO value
const int column_Sensor_Turbidity_value = column_Sensor_Turbidity_title + 8;      // column that used to display turbidity value
const int column_Sensor_Salinity_value = column_Sensor_Salinity_title + 8;        // column that used to display salinity value
const int column_Sensor_Depth_value = column_Sensor_Depth_title + 8;              // column that used to display depth value
// keypress
const int row_keypress = 1;                                    // row that used for keypress
const int first_keypress = 6;                                  // first column for keypress cursor
const int length_of_LCD_keypress_value = 16 - first_keypress;  // length of LCD keypress value

/* RTC */
// LED (status)
const int RTC_IndicatorLED_Green_PinNumber = 8;   // pin number of indicator LED (Green) of RTC in Arduino
const int RTC_IndicatorLED_Red_PinNumber   = 14;  // pin number of indicator LED (Red) of RTC in Arduino

/* KEYPAD */
// constants for rows and columns on the keypad
const byte numRows = 4;  // number of rows on the keypad
const byte numCols = 4;  // number of columns on the keypad
// keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {29, 28, 27, 26};  // rows 0 to 3    --- row    0..3 = pin 29..26
byte colPins[numCols] = {25, 24, 23, 22};  // columns 0 to 3 --- column 0..3 = pin 25..22
// initializes an instance of the keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

/* SD Card Module */
// pin number of ChipSelect (CS)
const int chipSelect = 53; // chip select
// LED (status)
const int SDCard_IndicatorLED_Green_PinNumber  = 15;  // pin number of indicator LED (Green)  of SD Card Module in Arduino
const int SDCard_IndicatorLED_Yellow_PinNumber = 16;  // pin number of indicator LED (Yellow) of SD Card Module in Arduino
const int SDCard_IndicatorLED_Red_PinNumber    = 17;  // pin number of indicator LED (Red)    of SD Card Module in Arduino

/* RECEIVER 433 MHz */
// pin number
const int Rx_PinNumber = 6; // pin number of Receiver 433 MHz in Arduino
// LED (status)
const int Rx_IndicatorLED_Green_PinNumber = 7; // pin number of indicator LED (Green) of Rx in Arduino
// data length of variable that used to receive data from RMP module
const int Rx_SensorData_ALL_Length          = 36;  // 6 x 6 length of data
const int Rx_SensorData_Temperature_Length  = 6;   // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_pH_Length           = 6;   // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_DO_Length           = 6;   // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_Turbidity_Length    = 6;   // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_Salinity_Length     = 6;   // 5 char for data, 1 char is empty for "end of data"
const int Rx_SensorData_Depth_Length        = 6;   // 5 char for data, 1 char is empty for "end of data"

/* GSM Module */
// pin configuration ALREADY on Library, so no need configuration in this section

/* Relay */
const int Relay_CH1_Kincir = 19;  // pin number for CH1 of Relay --- Kincir
const int Relay_CH2_Alarm  = 18;  // pin number for CH2 of Relay --- Alarm

/* etc. */
// LED (status) for sensor data
const int LED_SensorData_Temperature_Green  = 30;  // pin number of indicator LED (Green) of temperature status
const int LED_SensorData_Temperature_Red    = 31;  // pin number of indicator LED (Red)   of temperature status
const int LED_SensorData_pH_Green           = 32;  // pin number of indicator LED (Green) of pH status
const int LED_SensorData_pH_Red             = 33;  // pin number of indicator LED (Red)   of pH status
const int LED_SensorData_DO_Green           = 34;  // pin number of indicator LED (Green) of DO status
const int LED_SensorData_DO_Red             = 36;  // pin number of indicator LED (Red)   of DO status
const int LED_SensorData_Turbidity_Green    = 38;  // pin number of indicator LED (Green) of turbidity status
const int LED_SensorData_Turbidity_Red      = 40;  // pin number of indicator LED (Red)   of turbidity status
const int LED_SensorData_Salinity_Green     = 42;  // pin number of indicator LED (Green) of salinity status
const int LED_SensorData_Salinity_Red       = 44;  // pin number of indicator LED (Red)   of salinity status
const int LED_SensorData_Depth_Green        = 46;  // pin number of indicator LED (Green) of depth status
const int LED_SensorData_Depth_Red          = 48;  // pin number of indicator LED (Red)   of depth status
/*===========================================================================*/


/*===========================( DECLARE VARIABLES )===========================*/
/* General */
int MainCounter;

/* LCD and KEYPAD */
// keypress
char LCD_keypress_value[length_of_LCD_keypress_value];  // value of keypress
int  LCD_keypress_value_index;                          // index of LCD_keypress_value
int  LCD_i;                                             // counter of keypress
int  LCD_j;                                             // dummy variable for display keypress counter in serial monitor
// display of sensor data
int LCD_SensorData_Display;                             // flag for changing display of sensor data

/* RTC */
// set up variables using the RTC utility library functions
tmElements_t tm;
// common variable for RTC
int RTC_ThisDay;  // variable for checking and removing old file
int RTC_NextDay;  // variable for checking and removing old file

/* SD CARD MODULE */
// set up variables using the SD utility library functions
SdVolume  volume;
Sd2Card   card;
SdFile    root;
// variable for create, write, and delete file
File myFile1, myFile2;
// file name for data logging
char SDCard_FileName[12];  // file name: YYYYMMDD.TXT
// for make sure only one data will be written
int SDCard_DataLogging_Write;  // flag for writting process to SD card
// for marking the data that already received from RMP
int SDCard_Marker;
// for making new file for NEXT DAY
int SDCard_NewFile_NewDay_FLAG; // flag for creating new file on NEXT DAY

/* RECEIVER 433 MHz */
// variable for store value of sensor data from RMP module
String Rx_SensorData_Temperature;  // only store temperature value from RMP module
String Rx_SensorData_pH;           // only store pH value from RMP module
String Rx_SensorData_DO;           // only store DO value from RMP module
String Rx_SensorData_Turbidity;    // only store turbidity value from RMP module
String Rx_SensorData_Salinity;     // only store salinity value from RMP module
String Rx_SensorData_Depth;        // only store the depths of the pond value from RMP module

/* GSM Module */
SMSGSM  sms;                  //
boolean GSM_started = false;  // as flag for check the status of GSM Module, started or not yet
char    GSM_PhoneNumber[20];  // used to store Phone Number of destination
char    GSM_SMSText[160];     // used to store SMS content
int     GSM_CountNumber;      //

/* etc. */
// value of converted string data (from RMP) to FLOAT format
float Value_SensorData_Temperature;  // value of temperature
float Value_SensorData_pH;           // value of pH
float Value_SensorData_DO;           // value of DO
float Value_SensorData_Turbidity;    // value of turbidity
float Value_SensorData_Salinity;     // value of salinity
float Value_SensorData_Depth;        // value of depth
// value of converted string data (from RMP) to CHAR format
char Char_SensorData_Temperature[6];  // char of temperature value
char Char_SensorData_pH[6];           // char of pH value
char Char_SensorData_DO[6];           // char of DO value
char Char_SensorData_Turbidity[6];    // char of turbidity value
char Char_SensorData_Salinity[6];     // char of salinity value
char Char_SensorData_Depth[6];        // char of depth value
// status of Condition Check
// this status will be displayed on LCD
// description :
//   OK when NORMAL
//   -- when the value below normal
//   ++ when the value over normal
String ConditionCheck_Status_Temperature;  // status of temperature
String ConditionCheck_Status_pH;           // status of pH
String ConditionCheck_Status_DO;           // status of DO
String ConditionCheck_Status_Turbidity;    // status of turbidity
String ConditionCheck_Status_Salinity;     // status of salinity
String ConditionCheck_Status_Depth;        // status of depth
// flag for ERROR Condition Check
// description :
//   1 when ERROR
//   0 when NOT error
char ConditionCheck_Flag_ALL[7];               // flag for ALL parameters
char ConditionCheck_Flag_ALL_temp[7];          // flag for ALL parameters
int  ConditionCheck_Flag_Temperature;          // flag for temperature
int  ConditionCheck_Flag_pH;                   // flag for pH
int  ConditionCheck_Flag_DO;                   // flag for DO
int  ConditionCheck_Flag_Turbidity;            // flag for turbidity
int  ConditionCheck_Flag_Salinity;             // flag for salinity
int  ConditionCheck_Flag_Depth;                // flag for depth
boolean ConditionCheck_Flag_Comparation;       // comparation value of ALL flag
boolean ConditionCheck_Flag_Comparation_temp;  // temp for comparation value of ALL flag
/*===========================================================================*/


/*================================( SETUP )==================================*/
void setup()
{
  /* Serial Monitor */
  Serial.begin(9600);                               // initialize baud rate for Serial Monitor
  Serial.println(">> SETUP");

  /* Initialize the variables */
  // General
  MainCounter = 0;                                  // initialize the main counter
  // Receiver 433 MHz
  procedure_Rx_SensorData_Initialization();         // initialize the value of sensor data from RMP module
  // LCD 16x4 --- display of sensor data
  LCD_SensorData_Display   = 0;                     // flag for changing display of sensor data
  // LCD 16x4 --- keypress
  // procedure_Clear_LCD_keypress_value();             // reset or fill blank to LCD_keypress_value (replace with '_')
  // LCD_keypress_value_index = 1;                     // initialize the index (pointer) of LCD_keypress_value
  // LCD_i = first_keypress;                           // initialize the counter of keypress
  // LCD_j = 1;                                        // initialize the dummy variable for display keypress counter in serial monitor
  // RTC
  RTC_ThisDay = tm.Day;                             // initialize the variable for checking and removing old file
  RTC_NextDay = RTC_ThisDay + 1;                    // initialize the variable for checking and removing old file
  // Relay
  procedure_ConditionCheck_Flag_ALL_temp();         // initialize the flag variable for checking condition
  // SD Card
  SDCard_NewFile_NewDay_FLAG = 1;                   // flag for creating new file for NEXT DAY
  SDCard_DataLogging_Write = 0;                     // flag for writting process to SD card

  /* LED */
  procedure_LED_SystemStatus_SETUP_All();           // all pins of LED are set as OUTPUT, then each value assigned with 0

  /* Relay */
  procedure_Relay_SETUP();                          // all pins of Relay are set as OUTPUT, then each value assigned with 0

  /* LCD 16x4 */
  // set up the LCD's number of columns and rows
  lcd.begin(16, 4);                                 // sum of column = 16, sum of row = 4

  /* RECEIVER 433 MHz */
  procedure_Rx_SETUP();                             // initialize the Receiver 433 MHz

  /* RTC */
  procedure_RTC();                                  // check and display clock and date based on RTC module

  /* GSM Module */
  procedure_LCD_SETUP_GSM_start();                  //
  procedure_GSM_SETUP();
  delay(3000);                                       //
  procedure_GSM_SendMessage_Initialize();                 //
  procedure_LCD_SETUP_GSM_end();                    //

  /* LCD 16x4 */
  // process that using LCD
  procedure_LCD_SETUP_DateClock();                  // initialize the display of separator for date and clock
  procedure_LCD_SETUP_SystemStatus();               // initialize the display of text "Status:"
  procedure_LCD_SETUP_SensorData();                 // initialize the display of text "Suhu  =" and "pH    ="
  // procedure_LCD_SETUP_KeypressLabel();              // ...

  /* SD CARD MODULE */
  procedure_SDCard_Initialization();                // check the presence of SD card
  procedure_SDCard_FileAvailableCheck();            // check the availability of file name that named with date of today
}
/*===========================================================================*/


/*==============================( MAIN LOOP )================================*/
void loop()
{
  Serial.println();
  Serial.println(">> MAIN LOOP");

  // main counter
  procedure_Counter();                                          // counting every second, reset when reach 3600 seconds

  // print date and clock to LCD
  procedure_RTC();                                              // check and display clock and date based on RTC module

  /* REMOVE OLD FILE in SD CARD */
  // RTC_ThisDay = tm.Day;                                         // reset RTC_ThisDay variable with date of today
  // if (RTC_ThisDay == RTC_NextDay)                               // when value of RTC_ThisDay variable equal with RTC_NextDay variable
  // {
  // procedure_SDCard_RemoveOldFile();                           // REMOVE file one year before
  // procedure_SDCard_NewFile();                                 // create new file that named with YYYYMMDD of today
  // RTC_NextDay += 1;                                           // add value of RTC_NextDay variable
  // }

  if ((SDCard_NewFile_NewDay_FLAG == 1) && (tm.Hour % 22 == 0))  // when FLAG = 1 and time = 23:59:40
  {
    procedure_SDCard_NewFile_NewDay();
    SDCard_NewFile_NewDay_FLAG == 0;
  }

  if ((SDCard_NewFile_NewDay_FLAG == 0) && (tm.Hour % 23 == 0))  // when FLAG = 1 and time = 23:59:50
  {
    SDCard_NewFile_NewDay_FLAG == 1;
  }

  /* KEYPAD 4x4 */
  // procedure_LCD_Keypress();

  /* RECEIVER 433 MHz */
  procedure_Rx_ReceivingDataRMP();                              // receive data from RMP, then store it in variables
  procedure_SensorData_ConvertStringToFloat();                  // convert each string value of sensor data to float type
  procedure_SensorData_ConditionCheck();                        // compare those value with standard value (SNI)

  /* SENSOR DATA & LCD 16x4 */
  procedure_LCD_SensorData_Display();                           // display text of label, value, and status each parameter

  /* SD CARD MODULE */
  // condition for writing data to SD card
  if ((SDCard_DataLogging_Write == 0) && (tm.Second % 30 != 0)) // when the second of RTC can not devided by 2
  {
    SDCard_DataLogging_Write = 1;                               // CHANGE flag = 0 to flag = 1 for enabling writing data process
  }
  procedure_LED_SystemStatus_WriteFile_Clear();                 // clear the status of writing file
  // writing data to SD card
  if ((SDCard_DataLogging_Write == 1) && (tm.Second % 30 == 0)) // when the second of RTC CAN devided by 2
  {
    procedure_TEST_DisplayConvertStringToFloat();               // TESTING: display the converted value on serial monitor
    procedure_SDCard_FileName();                                // SDCard_FileName variable is assigned with the date of today
    procedure_SDCard_DataLogging();                             // write date of today, clock at that time, data sensor value, and SDCard_Marker value
    SDCard_DataLogging_Write = 0;                               // CHANGE flag = 1 to flag = 0 after writing data process finished
  }

  /* RELAY and GSM MODULE */
  procedure_ConditionCheck_Flag_Comparation(
    ConditionCheck_Flag_ALL,
    ConditionCheck_Flag_ALL_temp);                              // compare the value of each element
  procedure_Relay_CH2_Alarm_ON();                               // turn ON alarm and SMS when the ERROR condition come

}
/*===========================================================================*/


/*===========================( PROCEDURE DETAIL )============================*/
/*-------------------------------( General )---------------------------------*/
void procedure_Counter()
// input   : N/A
// output  : counter in integer
// process : 1. counting every 1 second
//           2. counting is reseted when reach 3600 seconds
{
  Serial.println(">> procedure_Counter()");

  if (MainCounter < 3600)
  {
    MainCounter = millis() / 1000;
  }
  else
  {
    MainCounter = 0;
  }
}

/*---------------------------------( LED )-----------------------------------*/
void procedure_LED_SystemStatus_SETUP_All()
// input   : N/A
// output  : 1. pin of LED are set as OUTPUT
//           2. value of each pin are assigned with 0
// process : 1. set all of pin for LED as OUTPUT
//           2. assign value of each pin with 0
{
  Serial.println(">> procedure_LED_SystemStatus_SETUP_All()");

  /* LED for DATA SENSOR */
  // set pin as output
  pinMode(LED_SensorData_Temperature_Green, OUTPUT);
  pinMode(LED_SensorData_Temperature_Red  , OUTPUT);
  pinMode(LED_SensorData_pH_Green         , OUTPUT);
  pinMode(LED_SensorData_pH_Red           , OUTPUT);
  pinMode(LED_SensorData_DO_Green         , OUTPUT);
  pinMode(LED_SensorData_DO_Red           , OUTPUT);
  pinMode(LED_SensorData_Turbidity_Green  , OUTPUT);
  pinMode(LED_SensorData_Turbidity_Red    , OUTPUT);
  pinMode(LED_SensorData_Salinity_Green   , OUTPUT);
  pinMode(LED_SensorData_Salinity_Red     , OUTPUT);
  /*
  pinMode(LED_SensorData_Depth_Green      , OUTPUT);
  pinMode(LED_SensorData_Depth_Red        , OUTPUT);
  */
  // assign pin value with 0
  digitalWrite(LED_SensorData_Temperature_Green, 0);
  digitalWrite(LED_SensorData_Temperature_Red  , 0);
  digitalWrite(LED_SensorData_pH_Green         , 0);
  digitalWrite(LED_SensorData_pH_Red           , 0);
  digitalWrite(LED_SensorData_DO_Green         , 0);
  digitalWrite(LED_SensorData_DO_Red           , 0);
  digitalWrite(LED_SensorData_Turbidity_Green  , 0);
  digitalWrite(LED_SensorData_Turbidity_Red    , 0);
  digitalWrite(LED_SensorData_Salinity_Green   , 0);
  digitalWrite(LED_SensorData_Salinity_Red     , 0);
  digitalWrite(LED_SensorData_Depth_Green      , 0);
  digitalWrite(LED_SensorData_Depth_Red        , 0);

  /* SD CARD MODULE */
  // set pin as output
  pinMode(SDCard_IndicatorLED_Green_PinNumber , OUTPUT);
  pinMode(SDCard_IndicatorLED_Yellow_PinNumber, OUTPUT);
  pinMode(SDCard_IndicatorLED_Red_PinNumber   , OUTPUT);
  // assign pin value with 0
  digitalWrite(SDCard_IndicatorLED_Green_PinNumber , 0);
  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber   , 0);

  /* RECEIVER 433 MHz */
  // set pin as output
  pinMode(Rx_IndicatorLED_Green_PinNumber     , OUTPUT);
  // assign pin value with 0
  digitalWrite(Rx_IndicatorLED_Green_PinNumber, 0);

  /* RTC */
  // set pin as output
  pinMode(RTC_IndicatorLED_Green_PinNumber, OUTPUT);
  pinMode(RTC_IndicatorLED_Red_PinNumber  , OUTPUT);
  // assign pin value with 0
  digitalWrite(RTC_IndicatorLED_Green_PinNumber, 0);
  digitalWrite(RTC_IndicatorLED_Red_PinNumber  , 0);
}

void procedure_LED_SystemStatus_WriteFile_Done()
// input   : N/A
// output  : green LED turned on
// process : 1. turn on the green LED for indicating SD card status
//           2. green LED turned on when writing file SUCCESS
{
  Serial.println(">> procedure_LED_SystemStatus_WriteFile_Done()");
  digitalWrite(SDCard_IndicatorLED_Green_PinNumber, 1);  // turn on the green LED for SD card
}

void procedure_LED_SystemStatus_WriteFile_Error()
// input   : N/A
// output  : red LED turned on
// process : 1. turn on the red LED for indicating SD card status
//           2. red LED turned on when writing file FAIL
{
  Serial.println(">> procedure_LED_SystemStatus_WriteFile_Error()");

  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 1);            // turn on the red LED for SD card

  lcd.setCursor(column_SystemStatus_content, row_SystemStatus);  // column = column_SystemStatus_content, row = row_SystemStatus
  lcd.print("writeERR");                                         // display text "writeERR" on LCD
}

void procedure_LED_SystemStatus_WriteFile_Clear()
// input   : N/A
// output  : ...
// process : ...
{
  Serial.println(">> procedure_LED_SystemStatus_WriteFile_Clear()");

  digitalWrite(SDCard_IndicatorLED_Green_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 0);
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 0);
}

void procedure_LED_SystemStatus_DeleteFile_Done()
// input   : N/A
// output  : yellow LED turned on
// process : 1. turn on the yellow LED for indicating SD card status
//           2. yellow LED turned on when deleting old file SUCCESS
{
  Serial.println(">> procedure_LED_SystemStatus_DeleteFile_Done()");

  digitalWrite(SDCard_IndicatorLED_Yellow_PinNumber, 1);  // turn on the red LED for SD card
}

void procedure_LED_SystemStatus_DeleteFile_Error()
// input   : N/A
// output  : 1. red LED turned on
//           2. display the text "del_ERR" on LCD
// process : 1. turn on the red LED for indicating SD card status
//           2. red LED turned on when deleting old file FAIL
//           3. display the text:
//              "del_ERR" when deleting old file fail
{
  Serial.println(">> procedure_LED_SystemStatus_DeleteFile_Error()");

  // LED
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 1);            // turn on the red LED for SD card
  // LCD
  lcd.setCursor(column_SystemStatus_content, row_SystemStatus);  // column = column_SystemStatus_content, row = row_SystemStatus
  lcd.print("del_ERR");                                          // display text "del_ERR" on LCD
}

void procedure_LED_SystemStatus_OpenFile_Error()
// input   : N/A
// output  : 1. red LED turned on
//           2. display the text "open_ERR" on LCD
// process : 1. turn on the red LED for indicating SD card status
//           2. red LED turned on when opening file FAIL
//           3. display the text:
//              "open_ERR" when deleting old file fail
{
  Serial.println(">> procedure_LED_SystemStatus_OpenFile_Error()");

  // LED
  digitalWrite(SDCard_IndicatorLED_Red_PinNumber, 1);            // turn on the red LED for SD card
  // LCD
  lcd.setCursor(column_SystemStatus_content, row_SystemStatus);  // column = column_SystemStatus_content, row = row_SystemStatus
  lcd.print("open_ERR");                                         // display text "open_ERR" on LCD
}

void procedure_LED_SystemStatus_RTC_Worked()
// input   : N/A
// output  : green LED turned on
// process : 1. turn on the green LED for indicating RTC status
//              (status: work or not work)
//           2. green LED turned on when RTC work properly
{
  Serial.println(">> procedure_LED_SystemStatus_RTC_Worked()");

  digitalWrite(RTC_IndicatorLED_Green_PinNumber, 1);  // turn on the green LED for RTC
}

void procedure_LED_SystemStatus_RTC_Error()
// input   : N/A
// output  : 1. red LED turned on
//           2. display the text "RTC_ERR1" or "RTC_ERR2" on LCD
// process : 1. turn on the red LED for indicating RTC status
//              (status: work or not work)
//           2. red LED turned on when RTC do not work properly
//           3. display the text:
//              "RTC_ERR1" when RTC chip is available, but hasn't SetUp yet
//              "RTC_ERR2" when RTC is not available, need check the circuitry
{
  Serial.println(">> procedure_LED_SystemStatus_RTC_Error()");

  if (RTC.chipPresent())                                           // RTC chip is available but hasn't SetUp yet
  {
    digitalWrite(RTC_IndicatorLED_Red_PinNumber, 1);               // turn on the red LED
    lcd.setCursor(column_SystemStatus_content, row_SystemStatus);  // column = column_SystemStatus_content, row = row_SystemStatus
    lcd.print("RTC_ERR1");                                         // display text "RTC_ERR1" on LCD
    // display on Serial Monitor
    Serial.println("The DS1307 is stopped.");
    Serial.println("Please run the SetTime example");
    Serial.println("to initialize the time and begin running.");
    Serial.println();
  }
  else                                                             // condition when error reading RTC
  {
    digitalWrite(RTC_IndicatorLED_Red_PinNumber, 1);               // turn on the red LED
    lcd.setCursor(column_SystemStatus_content, row_SystemStatus);  // column = column_SystemStatus_content, row = row_SystemStatus
    lcd.print("RTC_ERR2");                                         // display text "RTC_ERR2" on LCD
    // display on Serial Monitor
    Serial.println("DS1307 read error!");
    Serial.println("Please check the circuitry.");
    Serial.println();
  }
}

/*---------------------------( Receiver 433 MHz )----------------------------*/
void procedure_Rx_SETUP()
// input   : N/A
// output  : done the initialization for Receiver 433 MHz
// process : initialize the setting for Receiver 433 MHz
{
  Serial.println(">> procedure_Rx_SETUP()");

  vw_set_ptt_inverted(true);    // required for DR3100
  vw_set_rx_pin(Rx_PinNumber);  // pin that used for receiving data
  vw_setup(4000);               // bits per sec
  vw_rx_start();                // start the receiver PLL running
}

void procedure_Rx_SensorData_Initialization()
// input   : N/A
// output  : value of sensor data from RMP module are filled with "blank value"
// process : fill each variable that used for store value of sensor data from
//           RMP with 'space'
{
  Serial.println(">> procedure_Rx_SensorData_Initialization()");

  Rx_SensorData_Temperature  = "     ";
  Rx_SensorData_pH           = "     ";
  Rx_SensorData_DO           = "     ";
  Rx_SensorData_Turbidity    = "     ";
  Rx_SensorData_Salinity     = "     ";
  Rx_SensorData_Depth        = "     ";
}

void procedure_Rx_ReceivingDataRMP()
// input   : N/A
// output  : 1. data from RMP module are received
//           2. those data are assigned to variable that
//              used to store sensor data
// process : 1. turn on the green LED
//           2. set SDCard_Marker value as 1
//              (it means that data is new or already updated)
//           3. assign the variable that used to store sensor data
//              with data from RMP
{
  Serial.println(">> procedure_Rx_ReceivingDataRMP()");



  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen))                    // Non-blocking
  {
    /* initialize */
    digitalWrite(Rx_IndicatorLED_Green_PinNumber, 1);  // turn ON the green LED for Receiver 433 MHz
    SDCard_Marker = 1;                                 // set SDCard_Marker value as 1

    /* sensor data of TEMPERATURE */
    Serial.print("Temp  = ");                           // serial monitor (Arduino IDE)
    for (int i = 0; i <= 4; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Temperature[i] = ' ';
      Rx_SensorData_Temperature[i] = buf[i];
    }
    Serial.println();

    /* sensor data of PH */
    Serial.print("pH    = ");                           // serial monitor (Arduino IDE)
    for (int i = 5; i <= 9; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_pH[i - 5] = ' ';
      Rx_SensorData_pH[i - 5] = buf[i];
    }
    Serial.println();

    /* sensor data of DO */
    Serial.print("DO    = ");                           // serial monitor (Arduino IDE)
    for (int i = 10; i <= 14; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_DO[i - 10] = ' ';
      Rx_SensorData_DO[i - 10] = buf[i];
    }
    Serial.println();

    /* sensor data of TURBIDITY */
    Serial.print("Keruh = ");                           // serial monitor (Arduino IDE)
    for (int i = 15; i <= 19; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Turbidity[i - 15] = ' ';
      Rx_SensorData_Turbidity[i - 15] = buf[i];
    }
    Serial.println();

    /* sensor data of SALINITY */
    Serial.print("Garam = ");                           // serial monitor (Arduino IDE)
    for (int i = 20; i <= 24; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Salinity[i - 20] = ' ';
      Rx_SensorData_Salinity[i - 20] = buf[i];
    }
    Serial.println();

    /* sensor data of DEPTH */
    Serial.print("Dalam = ");                           // serial monitor (Arduino IDE)
    for (int i = 25; i <= 29; i++)
    {
      Serial.write(buf[i]);
      Rx_SensorData_Depth[i - 25] = ' ';
      Rx_SensorData_Depth[i - 25] = ' ';
    }
    Serial.println();

    digitalWrite(Rx_IndicatorLED_Green_PinNumber, 0);  // turn OFF the green LED for Receiver 433 MHz
  }
  //  Serial.println("tidak ada koneksi");
}

/*-------------------------------( LCD 16x4 )--------------------------------*/
void procedure_LCD_SETUP_GSM_start()
// input   :
// output  :
// process :
{
  Serial.println(">> procedure_LCD_SETUP_GSM_start()");

  // display the status for GSM
  lcd.setCursor(0, 0);            // column = 0, row = 0
  lcd.print("================");
  lcd.setCursor(0, 1);            // column = 0, row = 1
  lcd.print("  Inisialisasi  ");
  lcd.setCursor(0, 2);            // column = 0, row = 2
  lcd.print("   GSM Module   ");
  lcd.setCursor(0, 3);            // column = 0, row = 3
  lcd.print("================");
}

void procedure_LCD_SETUP_GSM_star_success()
// input   :
// output  :
// process :
{
  Serial.println(">> procedure_LCD_SETUP_GSM_start_success()");

  // display the status for GSM
  lcd.setCursor(0, 0);            // column = 0, row = 0
  lcd.print("================");
  lcd.setCursor(0, 1);            // column = 0, row = 1
  lcd.print("  Inisialisasi  ");
  lcd.setCursor(0, 2);            // column = 0, row = 2
  lcd.print("    BERHASIL    ");
  lcd.setCursor(0, 3);            // column = 0, row = 3
  lcd.print("================");
}

void procedure_LCD_SETUP_GSM_star_failed()
// input   :
// output  :
// process :
{
  Serial.println(">> procedure_LCD_SETUP_GSM_start_failed()");

  // display the status for GSM
  lcd.setCursor(0, 0);            // column = 0, row = 0
  lcd.print("================");
  lcd.setCursor(0, 1);            // column = 0, row = 1
  lcd.print("  Inisialisasi  ");
  lcd.setCursor(0, 2);            // column = 0, row = 2
  lcd.print("     GAGAL      ");
  lcd.setCursor(0, 3);            // column = 0, row = 3
  lcd.print("================");
}

void procedure_LCD_SETUP_GSM_end()
// input   :
// output  :
// process :
{
  Serial.println(">> procedure_LCD_SETUP_GSM_end()");

  // display the status for GSM
  lcd.setCursor(0, 0);            // column = 0, row = 0
  lcd.print("                ");  // CLEAR screen
  lcd.setCursor(0, 1);            // column = 0, row = 1
  lcd.print("                ");  // CLEAR screen
  lcd.setCursor(0, 2);            // column = 0, row = 2
  lcd.print("                ");  // CLEAR screen
  lcd.setCursor(0, 3);            // column = 0, row = 3
  lcd.print("                ");  // CLEAR screen
}

void procedure_LCD_SETUP_DateClock()
// input   : N/A
// output  : display of separator of date and clock
// process : 1. initialize number of columns and rows of LCD
//           2. print separator that used to display date
//           3. print separator that used to display clock
{
  Serial.println(">> procedure_LCD_SETUP_DateClock()");

  // date
  lcd.setCursor(first_date_cursor + 2, row_date);    // column = first_date_cursor + 2, row = row_date
  lcd.print("-");
  lcd.setCursor(first_date_cursor + 5, row_date);    // column = first_date_cursor + 5, row = row_date
  lcd.print("-");
  // clock
  lcd.setCursor(first_clock_cursor + 2, row_clock);  // column = first_clock_cursor + 2, row = row_date
  lcd.print(":");
}

void procedure_LCD_SETUP_SystemStatus()
// input   : N/A
// output  : display the text "Status:"
// process : print text "Status:"
{
  Serial.println(">> procedure_LCD_SETUP_SystemStatus()");

  lcd.setCursor(column_SystemStatus_title, row_SystemStatus);  // column = column_SystemStatus_title, row = row_SystemStatus
  lcd.print("Status:");                                        // display text "Status:" on LCD
}

void procedure_LCD_SETUP_SensorData()
// input   : N/A
// output  : display the text "Suhu  =" and "pH    =" on LCD
// process : 1. print text "Suhu  ="
//           2. print text "pH    ="
{
  Serial.println(">> procedure_LCD_SETUP_SensorData()");

  // temperature
  lcd.setCursor(column_Sensor_Temperature_title, row_Sensor_Temperature);  // column = column_Sensor_Temperature_title, row = row_Sensor_Temperature
  lcd.print("Suhu  =");                                                    // display text "Suhu  =" on LCD
  // pH
  lcd.setCursor(column_Sensor_pH_title, row_Sensor_pH);                    // column = column_Sensor_pH_title, row = row_Sensor_pH
  lcd.print("pH    =");                                                    // display text "pH    =" on LCD
}

void procedure_LCD_SETUP_KeypressLabel() // temporarly NOT USED because DATA SENSOR testing
// input   : ...
// output  : ...
// process : ...
// NOTE	   : DO NOT DELETE this procedure!
//           store this procedure in SETUP section
{
  Serial.println(">> procedure_LCD_SETUP_KeypressLabel()");

  // print label to the LCD
  // keypress label
  lcd.setCursor(0, row_keypress);               // column 0, row = row_keypress
  lcd.print("Ketik");                           // display text "Ketik" on LCD
  lcd.setCursor(first_keypress, row_keypress);  // column = first_keypress, row = row_keypress
  procedure_ClearScreen_keypress();             // clear the screen of keypress label
}

void procedure_LCD_Keypress() // temporarly NOT USED because DATA SENSOR testing
// input   : ...
// output  : ...
// process : ...
// NOTE	   : DO NOT DELETE this procedure!
//           store this procedure in MAIN LOOP section
{
  Serial.println(">> procedure_LCD_Keypress()");

  char keypressed = myKeypad.getKey();

  /* KEYPAD */
  // If key is pressed, this key is stored in 'keypressed' variable.
  // If key is not equal to 'NO_KEY', then this key is printed out.
  // If count = 17, then count is reset back to 0
  // (this means no key is pressed during the whole keypad scan process).
  if ((keypressed != NO_KEY) && (LCD_i <= 15))
  {
    // display the keypress on LCD
    lcd.setCursor(LCD_i, row_keypress);                         // column = LCD_i, row = row_keypress
    LCD_keypress_value[LCD_keypress_value_index] = keypressed;  // fill LCD_keypress_value with keypressed
    lcd.print(LCD_keypress_value[LCD_keypress_value_index]);    // print LCD_keypress_value
    LCD_keypress_value_index++;
    LCD_i++;
    LCD_j++;
  }
  else if ((keypressed != NO_KEY) && (LCD_i > 15))
  {
    // reset some variable
    LCD_i = first_keypress;                                     // reset LCD_i back to first_keypress
    LCD_keypress_value_index = 1;                               // reset the index back to 1
    procedure_Clear_LCD_keypress_value();                       // reset or fill blank to LCD_keypress_value (replace with '_')

    // clear-screen the keypress label on LCD
    lcd.setCursor(LCD_i, row_keypress);                         // column LCD_i, row = row_keypress
    procedure_ClearScreen_keypress();                           // clear the screen of keypress label

    // display the keypress on LCD (same with before)
    lcd.setCursor(LCD_i, row_keypress);                         // column = LCD_i, row = row_keypress
    LCD_keypress_value[LCD_keypress_value_index] = keypressed;  // fill LCD_keypress_value with keypressed
    lcd.print(LCD_keypress_value[LCD_keypress_value_index]);    // print LCD_keypress_value
    LCD_keypress_value_index++;
    LCD_i++;
    LCD_j++;
  }
}

void procedure_LCD_SensorData_Display()
// input   : N/A
// output  : 1. display text of each parameter
//           2. display text of value of each parameter
//           3. display text of status of each parameter
// process : display of these things sequentially:
//            1. label of parameter
//            2. value of each parameter
//            3. status of each parameter
{
  Serial.println(">> procedure_LCD_SensorData_Display()");

  if ((LCD_SensorData_Display == 0) && (MainCounter % 6 == 0))                   // when MainCounter value mod 6 equal 0
  {
    // temperature
    lcd.setCursor(column_Sensor_Temperature_title, row_Sensor_Temperature);      // column = column_Sensor_Temperature_title, row = row_Sensor_Temperature
    lcd.print("Suhu  =");                                                        // display text "Suhu  =" on LCD
    lcd.setCursor(column_Sensor_Temperature_value, row_Sensor_Temperature);      // column = column_Sensor_Temperature_value, row = row_Sensor_Temperature
    lcd.print(Rx_SensorData_Temperature);                                        // display value of temperature
    lcd.setCursor(column_Sensor_Temperature_value + 6, row_Sensor_Temperature);  // column = column_Sensor_Temperature_value + 7, row = row_Sensor_Temperature
    lcd.print(ConditionCheck_Status_Temperature);                                // display status (normal or not) of temperature
    // pH
    lcd.setCursor(column_Sensor_pH_title, row_Sensor_pH);                        // column = column_Sensor_pH_title, row = row_Sensor_pH
    lcd.print("pH    =");                                                        // display text "pH    =" on LCD
    lcd.setCursor(column_Sensor_pH_value, row_Sensor_pH);                        // column = column_Sensor_pH_value, row = row_Sensor_pH
    lcd.print(Rx_SensorData_pH);                                                 // display value of pH
    lcd.setCursor(column_Sensor_pH_value + 6, row_Sensor_pH);                    // column = column_Sensor_pH_value + 7, row = row_Sensor_pH
    lcd.print(ConditionCheck_Status_pH);                                         // display status (normal or not) of pH

    LCD_SensorData_Display ++;
  }
  // display other data sensor
  if ((LCD_SensorData_Display == 1) && (MainCounter % 6 == 2))                   // when MainCounter value mod 6 equal 2
  {
    // DO
    lcd.setCursor(column_Sensor_DO_title, row_Sensor_DO);                        // column = column_Sensor_DO_title, row = row_Sensor_DO
    lcd.print("DO    =");                                                        // display text "DO    =" on LCD
    lcd.setCursor(column_Sensor_DO_value, row_Sensor_DO);                        // column = column_Sensor_DO_value, row = row_Sensor_DO
    lcd.print(Rx_SensorData_DO);                                                 // display value of DO
    lcd.setCursor(column_Sensor_DO_value + 6, row_Sensor_DO);                    // column = column_Sensor_DO_value + 7, row = row_Sensor_DO
    lcd.print(ConditionCheck_Status_DO);                                         // display status (normal or not) of DO
    // turbidity
    lcd.setCursor(column_Sensor_Turbidity_title, row_Sensor_Turbidity);          // column = column_Sensor_Turbidity_title, row = row_Sensor_Turbidity
    lcd.print("Keruh =");                                                        // display text "Keruh =" on LCD
    lcd.setCursor(column_Sensor_Turbidity_value, row_Sensor_Turbidity);          // column = column_Sensor_Turbidity_value, row = row_Sensor_Turbidity
    lcd.print(Rx_SensorData_Turbidity);                                          // display value of turbidity
    lcd.setCursor(column_Sensor_Turbidity_value + 6, row_Sensor_Turbidity);      // column = column_Sensor_Turbidity_value + 7, row = row_Sensor_Turbidity
    lcd.print(ConditionCheck_Status_Turbidity);                                  // display status (normal or not) of turbidity

    LCD_SensorData_Display++;
  }
  // display other data sensor
  if ((LCD_SensorData_Display == 2) && (MainCounter % 6 == 4))                   // when MainCounter value mod 6 equal 4
  {
    // salinity
    lcd.setCursor(column_Sensor_Salinity_title, row_Sensor_Salinity);            // column = column_Sensor_Salinity_title, row = row_Sensor_Salinity
    lcd.print("Garam =");                                                        // display text "Garam =" on LCD
    lcd.setCursor(column_Sensor_Salinity_value, row_Sensor_Salinity);            // column = column_Sensor_Salinity_value, row = row_Sensor_Salinity
    lcd.print(Rx_SensorData_Salinity);                                           // display value of salinity
    lcd.setCursor(column_Sensor_Salinity_value + 6, row_Sensor_Salinity);        // column = column_Sensor_Salinity_value + 7, row = row_Sensor_Salinity
    lcd.print(ConditionCheck_Status_Salinity);                                   // display status (normal or not) of salinity
    // depth
    lcd.setCursor(column_Sensor_Depth_title, row_Sensor_Depth);                  // column = column_Sensor_Depth_title, row = row_Sensor_Depth
    lcd.print("       ");                                                        // display text "Dalam =" on LCD
    lcd.setCursor(column_Sensor_Depth_value, row_Sensor_Depth);                  // column = column_Sensor_Depth_value, row = row_Sensor_Depth
    lcd.print(Rx_SensorData_Depth);                                              // display value of depth
    lcd.setCursor(column_Sensor_Depth_value + 6, row_Sensor_Depth);              // column = column_Sensor_Depth_value + 7, row = row_Sensor_Depth
    lcd.print(ConditionCheck_Status_Depth);                                      // display status (normal or not) of depth

    LCD_SensorData_Display = 0 ;
  }
}

void procedure_Clear_LCD_keypress_value()
// input   : N/A
// output  : LCD_keypress_value variable is filled with '_'
// process : replace LCD_keypress_value variable with '_'
{
  Serial.println(">> procedure_Clear_LCD_keypress_value()");

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
  Serial.println(">> procedure_ClearScreen_keypress()");

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

/*----------------------------------( RTC )----------------------------------*/
void procedure_RTC()
// input   : N/A
// output  : display of clock and date
// process : 1. check clock and date from RTC module
//           2. display the clock and date from RTC module
{
  Serial.println(">> procedure_RTC()");

  if (RTC.read(tm))                                      // check the condition of RTC function
  {
    /* LED for RTC Status */
    procedure_LED_SystemStatus_RTC_Worked();             // green LED turned on when RTC work properly

    /* Print DATE to LCD */
    // Day [1..31]
    if (tm.Day < 10)                                     // condition for date number < 10
    {
      lcd.setCursor(first_date_cursor, row_date);        // column = first_date_cursor, row = row_date
      lcd.print('0');                                    // add text '0' on LCD
      lcd.setCursor(first_date_cursor + 1, row_date);    // column = first_date_cursor + 1, row = row_date
      lcd.print(tm.Day);                                 // display the date number [1..9] on LCD
    }
    else                                                 // condition for date number >= 10
    {
      lcd.setCursor(first_date_cursor, row_date);        // column = first_date_cursor, row = row_date
      lcd.print(tm.Day);                                 // display the date number [10..31] on LCD
    }

    // Month [1..12]
    if (tm.Month < 10)                                   // condition for month number < 10
    {
      lcd.setCursor(first_date_cursor + 3, row_date);    // column = first_date_cursor + 3, row = row_date
      lcd.print('0');                                    // add text '0' on LCD
      lcd.setCursor(first_date_cursor + 4, row_date);    // column = first_date_cursor + 4, row = row_date
      lcd.print(tm.Month);                               // display the month number [1..9] on LCD
    }
    else                                                 // condition for month number >= 10
    {
      lcd.setCursor(first_date_cursor + 3, row_date);    // column = first_date_cursor + 3, row = row_date
      lcd.print(tm.Month);                               // display the month number [10..12] on LCD
    }

    // Year [xxxx]
    lcd.setCursor(first_date_cursor + 6, row_date);      // column = first_date_cursor + 6, row = row_date
    lcd.print(tmYearToCalendar(tm.Year));                // display the year number [xxxx] on LCD

    /* Print CLOCK to LCD */
    // Hour [0..23]
    if (tm.Hour < 10)                                    // condition for hour number < 10
    {
      lcd.setCursor(first_clock_cursor, row_clock);      // column = first_clock_cursor, row = row_clock
      lcd.print('0');                                    // add text '0' on LCD
      lcd.setCursor(first_clock_cursor + 1, row_clock);  // column = first_clock_cursor + 1, row = row_clock
      lcd.print(tm.Hour);                                // display the hour number [0..9] on LCD
    }
    else                                                 // condition for hour number >= 10
    {
      lcd.setCursor(first_clock_cursor, row_clock);      // column = first_clock_cursor, row = row_clock
      lcd.print(tm.Hour);                                // display the hour number [10..23] on LCD
    }

    // Minute [0..59]
    if (tm.Minute < 10)                                  // condition for minute number < 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock);  // column = first_clock_cursor + 3, row 3 = row_clock
      lcd.print('0');                                    // add text '0' on LCD
      lcd.setCursor(first_clock_cursor + 4, row_clock);  // column = first_clock_cursor + 4, row 3 = row_clock
      lcd.print(tm.Minute);                              // display the minute number [0..9] on LCD
    }
    else                                                 // condition for minute number >= 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock);  // column = first_clock_cursor + 3, row = row_clock
      lcd.print(tm.Minute);                              // display the minute number [10..59] on LCD
    }
  }
  else
  {
    procedure_LED_SystemStatus_RTC_Error();              // red LED turned on when RTC do not work properly
  }
}

/*--------------------------------( SD Card )--------------------------------*/
void procedure_SDCard_Initialization()
// input   : N/A
// output  : status of SD card
// process : 1. check the presence of SD card
//           2. display the status on serial monitor
{
  Serial.println(">> procedure_SDCard_Initialization()");

  /* Initializing */
  // serial monitor
  Serial.println("#2");
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect))  // SD card is not available or not detected
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void procedure_SDCard_FileName()
// input   : N/A
// output  : SDCard_FileName variable is assigned with date of today
// process : 1. check date of today
//           2. assign SDCard_FileName variable with date of today
{
  Serial.println(">> procedure_SDCard_FileName()");

  // local variable
  int YYYY;
  int MM;
  int DD;

  /* Create File Name */
  // initializing
  YYYY = tmYearToCalendar(tm.Year);                          // year of today
  MM = tm.Month;                                             // month of today
  DD = tm.Day;                                               // date of today

  if ((MM < 10) && (DD < 10))                                // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SDCard_FileName, "%d0%d0%d.TXT", YYYY, MM, DD);  // assign SDCard_FileName variable with YYYYMMDD.TXT
  }
  else if ((MM < 10) && (DD >= 10))                          // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SDCard_FileName, "%d0%d%d.TXT", YYYY, MM, DD);   // assign SDCard_FileName variable with YYYYMMDD.TXT
  }
  else if ((MM >= 10) && (DD < 10))                          // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SDCard_FileName, "%d%d0%d.TXT", YYYY, MM, DD);   // assign SDCard_FileName variable with YYYYMMDD.TXT
  }
  else                                                       // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SDCard_FileName, "%d%d%d.TXT", YYYY, MM, DD);    // assign SDCard_FileName variable with YYYYMMDD.TXT
  }
}

void procedure_SDCard_FileAvailableCheck()
// input   : N/A
// output  : done checking the availability of file name
//           that named by YYYYMMDD of today
// process : 1. assign SDCard_FileName variable
//           2. check the presence of filename that named with YYYYMMDD of today
//           3. when that filename not exist,
//              delete old file, then create new file that named with YYYYMMDD of today
{
  Serial.println(">> procedure_SDCard_FileAvailableCheck()");

  procedure_SDCard_FileName();         // SDCard_FileName variable is assigned with the date of today
  // check the presence of filename that named with YYYYMMDD of today
  if (!SD.exists(SDCard_FileName))     // filename that named with YYYYMMDD of today is not exist
  {
    procedure_SDCard_RemoveOldFile();  // delete old file
    procedure_SDCard_NewFile();        // then, create new file that named with YYYYMMDD of today
  }
}

void procedure_SDCard_RemoveOldFile()
// input   : N/A
// output  : old files removed
// process : 1. assign FileName variable with name of old file (1 year before)
//           2. removing the old files
{
  Serial.println(">> procedure_SDCard_RemoveOldFile()");

  // local variable
  int YYYY;
  int MM;
  int DD;
  char FileName[12];

  /* Delete File Name */
  // initializing
  YYYY = tmYearToCalendar(tm.Year) - 1;               // 1 year BEFORE of today
  MM = tm.Month;                                      // month of today
  DD = tm.Day;                                        // date of today

  // define the file name of OLD FILE
  if ((MM < 10) && (DD < 10))                         // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d0%d0%d.TXT", YYYY, MM, DD);  // assign FileName variable with YYYYMMDD.TXT
  }
  else if ((MM < 10) && (DD >= 10))                   // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d0%d%d.TXT", YYYY, MM, DD);   // assign FileName variable with YYYYMMDD.TXT
  }
  else if ((MM >= 10) && (DD < 10))                   // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d%d0%d.TXT", YYYY, MM, DD);   // assign FileName variable with YYYYMMDD.TXT
  }
  else                                                // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d%d%d.TXT", YYYY, MM, DD);    // assign FileName variable with YYYYMMDD.TXT
  }

  // check the file before removed
  if (SD.exists(FileName))                            // checking, the old file is exist or not, delete if exist
  {
    Serial.print("File ");
    Serial.print(FileName);
    Serial.println(" must be deleted!");
    // remove the old file
    SD.remove(FileName);                              // removing the old file
    // check that the file has removed or not
    if (SD.exists(FileName))                          // checking, the old file is STILL exist or not
    {
      Serial.print("Deleting ");
      Serial.print(FileName);
      Serial.println(" FAILED!");
      procedure_LED_SystemStatus_DeleteFile_Error();  // turn on the red LED for SD card
    }
    else
    {
      Serial.print("Deleting ");
      Serial.print(FileName);
      Serial.println(" COMPLETED!");
      procedure_LED_SystemStatus_DeleteFile_Done();   // turn on the yellow LED for SD card
    }
  }
  else
  {
    Serial.println("No file need to be deleted!");
  }
}

void procedure_SDCard_NewFile()
// input   : N/A
// output  : 1. new file that named with YYYYMMDD of today is created
//           2. there is 1 line of title in that file
// process : 1. open YYYYMMDD.TXT file
//           2. write the title in 1st line of that file
{
  Serial.println(">> procedure_SDCard_NewFile()");

  /* NEW FILE for SENSOR DATA LOGGING - YYYYMMDD.TXT */
  // open the file
  // note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open YYYYMMDD.TXT file that already exist
  // if it isn't, then create YYYYMMDD.TXT as new file
  myFile1 = SD.open(SDCard_FileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1)                                    // file opened normally
  {
    // serial monitor
    Serial.print("Writing to ");
    Serial.print(SDCard_FileName);
    Serial.print("...");

    /* title */
    // title of the content of file
    myFile1.println("YYYYMMDD_hhmmss_Suhu _pH   _DO   _Turb _Salnt_Depth_FLAG ");

    // close the file
    myFile1.close();

    // serial monitor
    Serial.println("done.");
  }
  else                                            // the file do not opened normally
  {
    // if the file didn't open, print an error
    Serial.print("[NewFile] ERROR opening ");     // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.println("!");
    procedure_LED_SystemStatus_OpenFile_Error();  // turn on the red LED for SD card
  }
}

void procedure_SDCard_NewFile_NewDay()
// input   : N/A
// output  : 1. new file that named with YYYYMMDD of NEXT DAY is created
//           2. there is 1 line of title in that file
// process : 1. open YYYYMMDD.TXT file
//           2. write the title in 1st line of that file
{
  Serial.println(">> procedure_SDCard_NewFile_NewDay()");

  // local variable
  int YYYY;
  int MM;
  int DD;
  char FileName[12];

  /* Define File Name */
  // initializing
  YYYY = tmYearToCalendar(tm.Year);               // year of today
  MM = tm.Month;                                  // month of today
  DD = tm.Day + 1;                                // date of NEXT DAY

  // define the file name of OLD FILE
  if ((MM < 10) && (DD < 10))                         // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d0%d0%d.TXT", YYYY, MM, DD);  // assign FileName variable with YYYYMMDD.TXT
  }
  else if ((MM < 10) && (DD >= 10))                   // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d0%d%d.TXT", YYYY, MM, DD);   // assign FileName variable with YYYYMMDD.TXT
  }
  else if ((MM >= 10) && (DD < 10))                   // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d%d0%d.TXT", YYYY, MM, DD);   // assign FileName variable with YYYYMMDD.TXT
  }
  else                                                // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(FileName, "%d%d%d.TXT", YYYY, MM, DD);    // assign FileName variable with YYYYMMDD.TXT
  }

  /* NEW FILE for SENSOR DATA LOGGING - YYYYMMDD.TXT */
  // open the file
  // note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open YYYYMMDD.TXT file that already exist
  // if it isn't, then create YYYYMMDD.TXT as new file
  myFile1 = SD.open(FileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1)                                    // file opened normally
  {
    // serial monitor
    Serial.print("Writing to ");
    Serial.print(FileName);
    Serial.print("...");

    /* title */
    // title of the content of file
    myFile1.println("YYYYMMDD_hhmmss_Suhu _pH   _DO   _Turb _Salnt_FLAG ");

    // close the file
    myFile1.close();

    // serial monitor
    Serial.println("done.");
  }
  else                                            // the file do not opened normally
  {
    // if the file didn't open, print an error
    Serial.print("[NewFile] ERROR opening ");     // display on serial monitor (Arduino IDE)
    Serial.print(FileName);
    Serial.println("!");
    procedure_LED_SystemStatus_OpenFile_Error();  // turn on the red LED for SD card
  }
}

void procedure_SDCard_DataLogging()
// input   : N/A
// output  : data sensor are written on file
// process : 1. print date of today to file
//           2. print clock at that time
//           3. print data sensor value of each parameter
//           4. print SDCard_Marker value
{
  Serial.println(">> procedure_SDCard_DataLogging()");

  /* SENSOR DATA LOGGING - YYYYMMDD.TXT */
  // open the file. note that only one file can be open at a time,
  // so we have to close this one before opening another.

  // open YYYYMMDD.TXT file that already exist
  // if it isn't, then create YYYYMMDD.TXT as new file
  myFile1 = SD.open(SDCard_FileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile1)
  {
    Serial.print("Writing to ");                   // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.print("...");

    /* date */
    // Year
    myFile1.print(tmYearToCalendar(tm.Year));
    // Month
    if (tm.Month < 10)                             // condition for month number < 10
    {
      myFile1.print('0');                          // add text '0' before month number
      myFile1.print(tm.Month);                     // print month number to file
    }
    else                                           // condition for month number >= 10
    {
      myFile1.print(tm.Month);                     // print month number to file
    }
    // Day
    if (tm.Day < 10)                               // condition for date number < 10
    {
      myFile1.print('0');                          // add text '0' before date number
      myFile1.print(tm.Day);                       // print date number to file
    }
    else                                           // condition for date number >= 10
    {
      myFile1.print(tm.Day);                       // print date number to file
    }

    /* separator */
    myFile1.print('_');                            // print separator with 'underline'

    /* clock */
    // Hour
    if (tm.Hour < 10)                              // condition for hour number < 10
    {
      myFile1.print('0');                          // add text '0' before hour number
      myFile1.print(tm.Hour);                      // print hour number to file
    }
    else                                           // condition for hour number >= 10
    {
      myFile1.print(tm.Hour);                      // print hour number to file
    }
    // Minute
    if (tm.Minute < 10)                            // condition for minute number < 10
    {
      myFile1.print('0');                          // add text '0' before minute number
      myFile1.print(tm.Minute);                    // print minute number to file
    }
    else                                           // condition for minute number >= 10
    {
      myFile1.print(tm.Minute);                    // print minute number to file
    }
    // Second
    if (tm.Second < 10)                            // condition for second number < 10
    {
      myFile1.print('0');                          // add text '0' before second number
      myFile1.print(tm.Second);                    // print second number to file
    }
    else                                           // condition for second number >= 10
    {
      myFile1.print(tm.Second);                    // print second number to file
    }

    /* separator */
    myFile1.print('_');                            // print separator with 'underline'

    //    /* keypress */
    //    for (int i = 1; i <= length_of_LCD_keypress_value; i++)
    //    {
    //      myFile1.print(LCD_keypress_value[i]);
    //    }

    /* sensor data */
    myFile1.print(Rx_SensorData_Temperature);      // print temperature value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(Rx_SensorData_pH);               // print pH value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(Rx_SensorData_DO);               // print DO value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(Rx_SensorData_Turbidity);        // print turbidity value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(Rx_SensorData_Salinity);         // print salinity value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(Rx_SensorData_Depth);            // print depth value to file
    myFile1.print('_');                            // print separator with 'underline'
    myFile1.print(SDCard_Marker);                  // print SDCard_Marker value to file
    myFile1.println("");                           // change line

    // close the file
    myFile1.close();                               // close file

    Serial.println("done.");                       // display on serial monitor (Arduino IDE)

    // display status on LED
    procedure_LED_SystemStatus_WriteFile_Done();   // green LED turned on when writing file done

    // reset SDCard_Marker
    SDCard_Marker = 0;                             // set SDCard_Marker value as 0
  }
  else
  {
    // if the file didn't open, print an error
    Serial.print("[DataLogging] ERROR opening ");  // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.println("!");

    // display status on LED and LCD 16x4
    procedure_LED_SystemStatus_WriteFile_Error();  // red LED turned on and display text "writeERR" when error writing file
  }
}
/*--------------------------------( Relay )----------------------------------*/
// NOTE: Condition off the relay is ACTIVE LOW,
//       so HIGH = Relay OFF and LOW = Relay ON.
void procedure_Relay_SETUP()
// input   : N/A
// output  : 1. pin of Relay are set as OUTPUT
//           2. value of each pin are assigned with 0
// process : 1. set all of pin for Relay as OUTPUT
//           2. assign value of each pin with 0
{
  Serial.println(">> procedure_Relay_SETUP()");

  // set pin as output
  pinMode(Relay_CH1_Kincir, OUTPUT);
  pinMode(Relay_CH2_Alarm , OUTPUT);
  // assign pin value with 0
  digitalWrite(Relay_CH1_Kincir, 1);  // Relay = OFF
  digitalWrite(Relay_CH2_Alarm,  1);  // Relay = OFF
}

void procedure_Relay_CH2_Alarm_ON()
// input   : N/A
// output  : alarm ON
// process :
{
  Serial.println(">> procedure_Relay_CH2_Alarm_ON()");

  if (ConditionCheck_Flag_Comparation == false)
  {
    // turn ON alarm
    digitalWrite(Relay_CH2_Alarm, 0);  // Relay = ON
    delay(500);
    digitalWrite(Relay_CH2_Alarm, 1);  // Relay = OFF
    delay(200);
    digitalWrite(Relay_CH2_Alarm, 0);  // Relay = ON
    delay(500);
    digitalWrite(Relay_CH2_Alarm, 1);  // Relay = OFF
    delay(200);
    digitalWrite(Relay_CH2_Alarm, 0);  // Relay = ON
    delay(500);
    digitalWrite(Relay_CH2_Alarm, 1);  // Relay = OFF

    // send message using GSM Module
    procedure_GSM_SendMessage_main();
  }
}

/*------------------------------( GSM Module )-------------------------------*/
void procedure_GSM_SETUP()
// input   : N/A
// output  : ...
// process : ...
{
  Serial.println(">> procedure_GSM_SETUP()");

  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400))
  {
    procedure_LCD_SETUP_GSM_star_success();
    Serial.println("status=READY");
    GSM_started = true;
  }
  else
  {
    procedure_LCD_SETUP_GSM_star_failed();
    Serial.println("status=IDLE");
  }
  sprintf(GSM_PhoneNumber, "+6281214424227");
}

void procedure_GSM_SendMessage_Initialize()
// input   : N/A
// output  : alarm ON
// process : ...
{
  Serial.println(">> procedure_GSM_SendMessage_Initialize()");

  // local variable
  int YYYY;
  int MM;
  int DD;
  int hh;
  int mm;
  char SMS_Header[15];
  char SMS_Header_Date[9];
  char SMS_Header_Time[7];
  boolean flagSendMessage = true;

  /* SMS Header */
  // initializing
  YYYY = tmYearToCalendar(tm.Year);                      // year of today
  MM = tm.Month;                                         // month of today
  DD = tm.Day;                                           // date of today
  hh = tm.Hour;                                          // hour of now
  mm = tm.Minute;                                        // minute of now

  // date
  if ((MM < 10) && (DD < 10))                            // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d0%d0%d", YYYY, MM, DD);  // assign SMS_Header_Date variable with YYYYMMDD
  }
  else if ((MM < 10) && (DD >= 10))                      // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d0%d%d", YYYY, MM, DD);   // assign SMS_Header_Date variable with YYYYMMDD
  }
  else if ((MM >= 10) && (DD < 10))                      // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d%d0%d", YYYY, MM, DD);   // assign SMS_Header_Date variable with YYYYMMDD
  }
  else                                                   // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d%d%d.TXT", YYYY, MM, DD);     // assign SMS_Header_Date variable with YYYYMMDD
  }

  // time
  if ((hh < 10) && (mm < 10))                            // when (number of hour < 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "0%d0%d", hh, mm);          // assign SMS_Header_Time variable with hhmm
  }
  else if ((hh < 10) && (mm >= 10))                      // when (number of hour < 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "0%d%d", hh, mm);           // assign SMS_Header_Time variable with hhmm
  }
  else if ((hh >= 10) && (mm < 10))                      // when (number of hour >= 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "%d0%d", hh, mm);           // assign SMS_Header_Time variable with hhmm
  }
  else                                                   // when (number of hour >= 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "%d%d", hh, mm);                 // assign SMS_Header_Time variable with hhmm
  }

  // Header: date_time (YYYYMMDD_hhmm)
  sprintf(SMS_Header, "%s_%s", SMS_Header_Date, SMS_Header_Time);

  /* Send SMS */
  // CONVERT the variables that will be sent on SMS
  procedure_SensorData_ConvertStringToChar();

  if (flagSendMessage)
  {
    GSM_CountNumber = 0;

    sprintf(GSM_SMSText, "%s\nParameter Kolam(%d)\nSuhu = %s (%d)\npH = %s(%d)\nDO = %s(%d)\nKeruh = %s(%d)\nGaram = %s(%d)",
            SMS_Header, GSM_CountNumber,
            Char_SensorData_Temperature, ConditionCheck_Flag_Temperature,
            Char_SensorData_pH, ConditionCheck_Flag_pH,
            Char_SensorData_DO, ConditionCheck_Flag_DO,
            Char_SensorData_Turbidity, ConditionCheck_Flag_Turbidity,
            Char_SensorData_Salinity, ConditionCheck_Flag_Salinity);

    if (sms.SendSMS(GSM_PhoneNumber, GSM_SMSText))
    {
      Serial.print("Nomor: ");
      Serial.println(GSM_PhoneNumber);
      Serial.println("Isi  : ");
      Serial.println(GSM_SMSText);
      Serial.println("SMS sent OK!");
    }
    else
    {
      Serial.println("SMS sent FAIL!");
    }
  }
  else // do nothing
  {
  }

  flagSendMessage = false;
}

void procedure_GSM_SendMessage_main()
// input   : N/A
// output  : alarm ON
// process : ...
{
  Serial.println(">> procedure_GSM_SendMessage_main()");

  // local variable
  int YYYY;
  int MM;
  int DD;
  int hh;
  int mm;
  char SMS_Header[15];
  char SMS_Header_Date[9];
  char SMS_Header_Time[7];
  boolean flagSendMessage = true;

  /* SMS Header */
  // initializing
  YYYY = tmYearToCalendar(tm.Year);                      // year of today
  MM = tm.Month;                                         // month of today
  DD = tm.Day;                                           // date of today
  hh = tm.Hour;                                          // hour of now
  mm = tm.Minute;                                        // minute of now

  // date
  if ((MM < 10) && (DD < 10))                            // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d0%d0%d", YYYY, MM, DD);  // assign SMS_Header_Date variable with YYYYMMDD
  }
  else if ((MM < 10) && (DD >= 10))                      // when (number of month < 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d0%d%d", YYYY, MM, DD);   // assign SMS_Header_Date variable with YYYYMMDD
  }
  else if ((MM >= 10) && (DD < 10))                      // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d%d0%d", YYYY, MM, DD);   // assign SMS_Header_Date variable with YYYYMMDD
  }
  else                                                   // when (number of month >= 10) AND (number of date) < 10
  {
    sprintf(SMS_Header_Date, "%d%d%d.TXT", YYYY, MM, DD);     // assign SMS_Header_Date variable with YYYYMMDD
  }

  // time
  if ((hh < 10) && (mm < 10))                            // when (number of hour < 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "0%d0%d", hh, mm);          // assign SMS_Header_Time variable with hhmm
  }
  else if ((hh < 10) && (mm >= 10))                      // when (number of hour < 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "0%d%d", hh, mm);           // assign SMS_Header_Time variable with hhmm
  }
  else if ((hh >= 10) && (mm < 10))                      // when (number of hour >= 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "%d0%d", hh, mm);           // assign SMS_Header_Time variable with hhmm
  }
  else                                                   // when (number of hour >= 10) AND (number of minute) < 10
  {
    sprintf(SMS_Header_Time, "%d%d", hh, mm);                 // assign SMS_Header_Time variable with hhmm
  }

  // Header: date_time (YYYYMMDD_hhmm)
  sprintf(SMS_Header, "%s_%s", SMS_Header_Date, SMS_Header_Time);

  /* Send SMS */
  // CONVERT the variables that will be sent on SMS
  procedure_SensorData_ConvertStringToChar();

  if (flagSendMessage)
  {
    delay(1000);

    GSM_CountNumber += 1;

    sprintf(GSM_SMSText, "%s\nParameter Kolam(%d)\nSuhu = %s (%d)\npH = %s(%d)\nDO = %s(%d)\nKeruh = %s(%d)\nGaram = %s(%d)",
            SMS_Header, GSM_CountNumber,
            Char_SensorData_Temperature, ConditionCheck_Flag_Temperature,
            Char_SensorData_pH, ConditionCheck_Flag_pH,
            Char_SensorData_DO, ConditionCheck_Flag_DO,
            Char_SensorData_Turbidity, ConditionCheck_Flag_Turbidity,
            Char_SensorData_Salinity, ConditionCheck_Flag_Salinity);

    if (sms.SendSMS(GSM_PhoneNumber, GSM_SMSText))
    {
      Serial.print("Nomor: ");
      Serial.println(GSM_PhoneNumber);
      Serial.println("Isi  : ");
      Serial.println(GSM_SMSText);
      Serial.println("SMS sent OK!");
    }
    else
    {
      Serial.println("SMS sent FAIL!");
    }
  }
  else // do nothing
  {
  }

  flagSendMessage = false;
}

/*---------------------------------( etc. )----------------------------------*/
void procedure_SensorData_ConditionCheck()
// input   : N/A
// output  : 1. NORMAL value when condition is normal, OR
//           2. NOT NORMAL value when condition is not normal
// process : 1. check every data sensor that received from RMP module
//           2. compare those value with standard value (SNI)
{
  Serial.println(">> procedure_SensorData_ConditionCheck()");

  /* Standard-Value Based on Standar Nasional Indonesia (SNI) */
  // temperature (Celcius degree)
  float Temperature_Low  = 28.5;
  float Temperature_High = 31.5;

  // pH
  float pH_Low  = 7.5;
  float pH_High = 8.5;

  // DO (miligram per liter)
  float DO_Low = 3.5;

  // turbidity --- non SNI
  float Turbidity_Low  = 100;
  float Turbidity_High = 500;

  // salinity (gram per liter)
  float Salinity_Low  = 15;
  float Salinity_High = 25;

  // depth (centimeter)
  // float Depth_Low  = 120;
  // float Depth_High = 200;
  float Depth_Low  = 30;   // for TEST only, DELETE later
  float Depth_High = 200;  // for TEST only, DELETE later

  /* Condition Check */
  // #1
  // sensor data of TEMPERATURE
  if ((Value_SensorData_Temperature >= Temperature_Low) && (Value_SensorData_Temperature <= Temperature_High))
  {
    ConditionCheck_Flag_Temperature   = 0;              // FLAG when Temperature NORMAL
    ConditionCheck_Status_Temperature = "OK";           // when Temperature NORMAL
    digitalWrite(LED_SensorData_Temperature_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_Temperature_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_Temperature < Temperature_Low)
  {
    ConditionCheck_Flag_Temperature   = 1;              // FLAG when Temperature BELOW normal
    ConditionCheck_Status_Temperature = "--";           // when Temperature BELOW normal
    digitalWrite(LED_SensorData_Temperature_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Temperature_Red  , 1);  // turn ON the RED LED
  }
  else if (Value_SensorData_Temperature > Temperature_High)
  {
    ConditionCheck_Flag_Temperature   = 1;              // FLAG when Temperature OVER normal
    ConditionCheck_Status_Temperature = "++";           // when Temperature OVER normal
    digitalWrite(LED_SensorData_Temperature_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Temperature_Red  , 1);  // turn ON the RED LED
  }

  // #2
  // sensor data of PH
  if ((Value_SensorData_pH >= pH_Low) && (Value_SensorData_pH <= pH_High))
  {
    ConditionCheck_Flag_pH   = 0;              // FLAG when pH NORMAL
    ConditionCheck_Status_pH = "OK";           // when pH NORMAL
    digitalWrite(LED_SensorData_pH_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_pH_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_pH < pH_Low)
  {
    ConditionCheck_Flag_pH   = 1;              // FLAG when pH BELOW normal
    ConditionCheck_Status_pH = "--";           // when pH BELOW normal
    digitalWrite(LED_SensorData_pH_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_pH_Red  , 1);  // turn ON the RED LED
  }
  else if (Value_SensorData_pH > pH_High)
  {
    ConditionCheck_Flag_pH   = 1;              // FLAG when pH OVER normal
    ConditionCheck_Status_pH = "++";           // when pH OVER normal
    digitalWrite(LED_SensorData_pH_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_pH_Red  , 1);  // turn ON the RED LED
  }

  // #3
  // sensor data of DO
  if (Value_SensorData_DO >= DO_Low)
  {
    ConditionCheck_Flag_DO   = 0;              // FLAG when DO NORMAL
    ConditionCheck_Status_DO = "OK";           // when DO NORMAL
    digitalWrite(LED_SensorData_DO_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_DO_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_DO < DO_Low)
  {
    ConditionCheck_Flag_DO   = 1;              // FLAG when DO NOT normal
    ConditionCheck_Status_DO = "--";           // when DO BELOW normal
    digitalWrite(LED_SensorData_DO_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_DO_Red  , 1);  // turn ON the RED LED
  }

  // #4
  // sensor data of TURBIDITY
  if ((Value_SensorData_Turbidity >= Turbidity_Low) && (Value_SensorData_Turbidity <= Turbidity_High))
  {
    ConditionCheck_Flag_Turbidity   = 0;              // FLAG when Turbidity NORMAL
    ConditionCheck_Status_Turbidity = "OK";           // when Turbidity NORMAL
    digitalWrite(LED_SensorData_Turbidity_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_Turbidity_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_Turbidity < Turbidity_Low)
  {
    ConditionCheck_Flag_Turbidity   = 1;              // FLAG when Turbidity BELOW normal
    ConditionCheck_Status_Turbidity = "--";           // when Turbidity BELOW normal
    digitalWrite(LED_SensorData_Turbidity_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Turbidity_Red  , 1);  // turn ON the RED LED
  }
  else if (Value_SensorData_Turbidity > Turbidity_High)
  {
    ConditionCheck_Flag_Turbidity   = 1;              // FLAG when Turbidity OVER normal
    ConditionCheck_Status_Turbidity = "++";           // when Turbidity OVER normal
    digitalWrite(LED_SensorData_Turbidity_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Turbidity_Red  , 1);  // turn ON the RED LED
  }

  // #5
  // sensor data of SALINITY
  if ((Value_SensorData_Salinity >= Salinity_Low) && (Value_SensorData_Salinity <= Salinity_High))
  {
    ConditionCheck_Flag_Salinity   = 0;              // FLAG when Salinity NORMAL
    ConditionCheck_Status_Salinity = "OK";           // when Salinity NORMAL
    digitalWrite(LED_SensorData_Salinity_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_Salinity_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_Salinity < Salinity_Low)
  {
    ConditionCheck_Flag_Salinity   = 1;              // FLAG when Salinity BELOW normal
    ConditionCheck_Status_Salinity = "--";           // when Salinity BELOW normal
    digitalWrite(LED_SensorData_Salinity_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Salinity_Red  , 1);  // turn ON the RED LED
  }
  else if (Value_SensorData_Salinity > Salinity_High)
  {
    ConditionCheck_Flag_Salinity   = 1;              // FLAG when Salinity OVER normal
    ConditionCheck_Status_Salinity = "++";           // when Salinity OVER normal
    digitalWrite(LED_SensorData_Salinity_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Salinity_Red  , 1);  // turn ON the RED LED
  }

  // #6
  // sensor data of DEPTH
  if ((Value_SensorData_Depth >= Depth_Low) && (Value_SensorData_Depth <= Depth_High))
  {
    ConditionCheck_Flag_Depth   = 0;              // FLAG when Depth NORMAL
    ConditionCheck_Status_Depth = "  ";           // when Depth NORMAL
    digitalWrite(LED_SensorData_Depth_Green, 1);  // turn ON the GREEN LED
    digitalWrite(LED_SensorData_Depth_Red  , 0);  // turn OFF the RED LED
  }
  // when NOT normal
  else if (Value_SensorData_Depth < Depth_Low)
  {
    ConditionCheck_Flag_Depth   = 1;              // FLAG when Depth BELOW normal
    ConditionCheck_Status_Depth = "  ";           // when Depth BELOW normal
    digitalWrite(LED_SensorData_Depth_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Depth_Red  , 1);  // turn ON the RED LED
  }
  else if (Value_SensorData_Depth > Depth_High)
  {
    ConditionCheck_Flag_Depth   = 1;              // FLAG when Depth OVER normal
    ConditionCheck_Status_Depth = "  ";           // when Depth OVER normal
    digitalWrite(LED_SensorData_Depth_Green, 0);  // turn OFF the GREEN LED
    digitalWrite(LED_SensorData_Depth_Red  , 1);  // turn ON the RED LED
  }

  /* Concate the FLAG */
  sprintf(ConditionCheck_Flag_ALL, "%d%d%d%d%d%d", ConditionCheck_Flag_Temperature,
          ConditionCheck_Flag_pH,
          ConditionCheck_Flag_DO,
          ConditionCheck_Flag_Turbidity,
          ConditionCheck_Flag_Salinity,
          ConditionCheck_Flag_Depth);       // concate the FLAG of each parameters
  Serial.print("ConditionCheck_Flag_ALL = ");                                        // display on Serial Monitor (Arduino IDE)
  Serial.println(ConditionCheck_Flag_ALL);
}

void procedure_ConditionCheck_Flag_ALL_temp()
// input   : ...
// output  : ...
// process : ...
{
  Serial.println(">> procedure_ConditionCheck_Flag_ALL_temp()");

  for (int i = 0; i <= 5; i++)
  {
    ConditionCheck_Flag_ALL_temp[i] = '0';
  }

  Serial.print("ConditionCheck_Flag_ALL_temp = ");
  Serial.println(ConditionCheck_Flag_ALL_temp);
}

void procedure_ConditionCheck_Flag_Comparation(char Comparator1[7], char Comparator2[7])
// input   : 1. Comparator1 (type: char) as NOW flag
//           2. Comparator2 (type: char) as temporarily flag
// output  : TRUE/FALSE in ConditionCheck_Flag_Comparation variable
//           1. FALSE mean any CHANGE, so this condition will trigger ALARM
//           2. TRUE  mean NO change , so this condition will NOT trigger alarm
// process : compare the value of each element of Comparator1 and Comparator2
{
  Serial.println(">> procedure_ConditionCheck_Flag_Comparation()");

  /* Initialization */
  ConditionCheck_Flag_Comparation = true;

  Serial.print("Comparator1 = ");
  Serial.println(Comparator1);
  Serial.print("Comparator2 = ");
  Serial.println(Comparator2);

  /* Compare Condition */
  for (int i = 0; i <= 5; i++)
  {
    if (Comparator1[i] > Comparator2[i])
    {
      ConditionCheck_Flag_Comparation_temp = false;  // any CHANGE
    }
    else
    {
      ConditionCheck_Flag_Comparation_temp = true;   // NORMAL condition, no change
    }
    ConditionCheck_Flag_Comparation = ConditionCheck_Flag_Comparation &&
                                      ConditionCheck_Flag_Comparation_temp;
  }

  /* Display on Serial Monitor */
  if (ConditionCheck_Flag_Comparation == true)
  {
    Serial.println("ConditionCheck_Flag_Comparation = TRUE");
  }
  else
  {
    Serial.println("ConditionCheck_Flag_Comparation = FALSE");
  }

  /* Replace the OLD data to TEMP variable */
  for (int i = 0; i <= 5; i++)
  {
    Comparator2[i] = Comparator1[i];
  }
  Serial.print("Comparator2 (after REPLACING)   = ");
  Serial.println(Comparator2);
}

void procedure_SensorData_ConvertStringToFloat()
// input   : N/A
// output  : sensor data (string type) converted to FLOAT type
// process : convert each string value of sensor data to FLOAT type
{
  Serial.println(">> procedure_SensorData_ConvertStringToFloat()");

  Value_SensorData_Temperature  = Rx_SensorData_Temperature.toFloat();  // convert temperature value, STRING to FLOAT
  Value_SensorData_pH           = Rx_SensorData_pH.toFloat();           // convert pH value,          STRING to FLOAT
  Value_SensorData_DO           = Rx_SensorData_DO.toFloat();           // convert DO value,          STRING to FLOAT
  Value_SensorData_Turbidity    = Rx_SensorData_Turbidity.toFloat();    // convert turbidity value,   STRING to FLOAT
  Value_SensorData_Salinity     = Rx_SensorData_Salinity.toFloat();     // convert salinity value,    STRING to FLOAT
  Value_SensorData_Depth        = Rx_SensorData_Depth.toFloat();        // convert depth value,       STRING to FLOAT
}

void procedure_SensorData_ConvertStringToChar()
// input   : N/A
// output  : sensor data (string type) converted to CHAR type
// process : convert each string value of sensor data to CHAR type
{
  Serial.println(">> procedure_SensorData_ConvertStringToFloat()");

  Rx_SensorData_Temperature.toCharArray(Char_SensorData_Temperature, 6);  // convert temperature value, STRING to CHAR
  Rx_SensorData_pH.toCharArray(Char_SensorData_pH, 6);                    // convert pH value,          STRING to CHAR
  Rx_SensorData_DO.toCharArray(Char_SensorData_DO, 6);                    // convert DO value,          STRING to CHAR
  Rx_SensorData_Turbidity.toCharArray(Char_SensorData_Turbidity, 6);      // convert turbidity value,   STRING to CHAR
  Rx_SensorData_Salinity.toCharArray(Char_SensorData_Salinity, 6);        // convert salinity value,    STRING to CHAR
  Rx_SensorData_Depth.toCharArray(Char_SensorData_Depth, 6);              // convert depth value,       STRING to CHAR
}

void procedure_TEST_DisplayConvertStringToFloat()
// input   : N/A
// output  : converted value are displayed on serial monitor
// process : 1. display text of label of each parameter
//           2. display text of value of each parameter
// NOTE    : this procedure is used to check if the conversion
//           is succes or not
{
  Serial.println(">> procedure_TEST_DisplayConvertStringToFloat()");

  Serial.print("Temperature = ");
  Serial.println(Value_SensorData_Temperature);
  Serial.print("pH          = ");
  Serial.println(Value_SensorData_pH);
  Serial.print("DO          = ");
  Serial.println(Value_SensorData_DO);
  Serial.print("Turbidity   = ");
  Serial.println(Value_SensorData_Turbidity);
  Serial.print("Salinity    = ");
  Serial.println(Value_SensorData_Salinity);
  Serial.print("Depth       = ");
  Serial.println(Value_SensorData_Depth);
}

/*===========================================================================*/
