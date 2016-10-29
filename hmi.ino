/*******************************************************************
====================================================================
Project        : Experiment
                 Mixing LCD 16x4, Keypad 4x4, and RTC
Version        : 1.1
Date Created   : August 27, 2015
Date Modified  : September 1, 2015
Author         : Baharuddin Aziz (http://baha.web.id)
Company        : Department of Electrical Engineering
                 School of Electrical Engineering and Informatics
                 Bandung Institute of Technology (ITB)
Summary        : 1. This program for implement 4x4 Matrix Keypad
                    to LCD 16x4 that connected to Arduino board
					with RTC module.
                 2. Every keypress will be display on LCD 16x4, and
				    also the date and clock of the day.
====================================================================
                          Additional Info
--------------------------------------------------------------------
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
> Thanks to :
    1. Alexander Brevig (alexanderbrevig@gmail.com) for providing :
        - CustomKeypad source code example
    2. David A. Mellis (Arduino Team) for providing :
        - LiquidCrystal source code example
    3. Tom Igoe (Arduino Team) for providing :
        - LiquidCrystal source code example
    4. Limo Fried for providing :
        - LiquidCrystal source code example
    5. Michael Margolis for providing :
        - SetTime example code
        - ReadTest example code
====================================================================
                            Changelog
--------------------------------------------------------------------
v1.0  -
v1.1  1. Add a RTC module.
      2. Change the display of LCD. The display include:
          + Title and counter (in seconds)
          + Date [DD-MM-YYYY]
          + Clock [HH:MM:SS]
          + Keypress (for checking the keypad module)
*******************************************************************/

/*------------------------( Import needed libraries )-----------------------------*/
#include <LiquidCrystal.h> // include the LCD library code
#include <Keypad.h> // include the 4x4 matrix keypad library code
#include <Wire.h>
#include <Time.h> // include time library code
#include <DS1307RTC.h> // include the DS1307 RTC library code
/*---------------------------------------------------------------------------------*/

/*---------------------( Declare Constants and Pin Numbers )-----------------------*/
// LCD 16x4
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// constant for first keypress
const int first_title_counter = 0; // first number for title counter
const int first_keypress = 6; // first column for keypress cursor
const int first_date_cursor = 4; // first column for date cursor
const int first_clock_cursor = 4; // first column for clock cursor
const int row_keypress = 3; // row that used for keypress
const int row_date = 1; // row that used to display the date
const int row_clock = 2; // row that used to display the clock
const int column_title_counter = 11; // first column for title counter

// KEYPAD
// constants for rows and columns on the keypad
const byte numRows = 4; // number of rows on the keypad
const byte numCols = 4; // number of columns on the keypad

/* Keymap defines the key pressed according to
   the row and columns just as appears on the keypad. */
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
/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare objects )---------------------------------*/
/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare Variables )-------------------------------*/
//int title_counter; // counter (in seconds) at the title of LCD
int i; // counter for keypress in line 2
int j; // dummy variable for display keypress counter in serial monitor
/*---------------------------------------------------------------------------------*/

/*-----------------------------------( SETUP )-------------------------------------*/
void setup()
{
  // KEYPAD
  Serial.begin(9600);

  // LCD 16x4
  // set up the LCD's number of columns and rows
  lcd.begin(16, 4); // sum of column = 16, sum of row = 4

  // print title to the LCD
  lcd.print("HMI_v0.1_|_"); // default at row 1

  // print label to the LCD
  lcd.setCursor(0, row_keypress); // column 0, row 1
  lcd.print("Ketik");
  lcd.setCursor(0, row_date); // column 0, row 2
  lcd.print("Tgl");
  lcd.setCursor(first_date_cursor + 2, row_date); // column = first_date_cursor + 2, row = row_date
  lcd.print("-");
  lcd.setCursor(first_date_cursor + 5, row_date); // column = first_date_cursor + 5, row = row_date
  lcd.print("-");
  lcd.setCursor(0, row_clock); // column 0, row 3
  lcd.print("Jam");
  lcd.setCursor(first_date_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
  lcd.print(":");
  lcd.setCursor(first_date_cursor + 5, row_clock); // column = first_date_cursor + 5, row = row_clock
  lcd.print(":");

  // initialize the variable
  i = first_keypress;
  j = 1;
}
/*---------------------------------------------------------------------------------*/

/*--------------------------------( MAIN LOOP )------------------------------------*/
void loop()
{
  char keypressed = myKeypad.getKey();

  // counter after title of LCD
  procedure_counter();

  // print date and clock to LCD
  procedure_RTC();
  
  // KEYPAD
  // If key is pressed, this key is stored in 'keypressed' variable.
  // If key is not equal to 'NO_KEY', then this key is printed out.
  // If count = 17, then count is reset back to 0
  // (this means no key is pressed during the whole keypad scan process).

  if ((keypressed != NO_KEY) && (i <= 15))
  {
    lcd.setCursor(i, row_keypress); // column i, row 1 at LCD
    lcd.print(keypressed);
    i++;
    j++;
  }
  else if ((keypressed != NO_KEY) && (i > 15))
  {
    i = first_keypress;
    lcd.setCursor(i, row_keypress); // column i, row 1 at LCD
    lcd.print(keypressed);
    i++;
    j++;
  }

  if (i < j)
  {
    Serial.println(i);
  }
}
/*---------------------------------------------------------------------------------*/

/*-----------------------------( PROCEDURE DETAIL )--------------------------------*/
void procedure_counter()
// input   : N/A
// output  : counter in integer
// process : counting every 1 second
{
  int hitung;

  lcd.setCursor(column_title_counter, 0); // column = first_title_cursor, row 0

  hitung = 0; // initialization
  hitung = millis() / 1000;
  lcd.print(hitung);
}

void procedure_RTC()
// input   : N/A
// output  : display of clock and date
// process : check clock and date from RTC module
{
  tmElements_t tm;
  if (RTC.read(tm))
  {
    // print date to LCD
    // date [1..31]
    if (tm.Day < 10)
    {
      lcd.setCursor(first_date_cursor, row_date); // column = first_date_cursor, row = row_date
      lcd.print('0'); // day [1..31]
      lcd.setCursor(first_date_cursor + 1 , row_date); // column = first_date_cursor + 1, row = row_date
      lcd.print(tm.Day); // day [1..31]
    } else
    {
      lcd.setCursor(first_date_cursor, row_date); // column = first_date_cursor, row = row_date
      lcd.print(tm.Day); // day [1..31]
    }

    // month [1..12]
    if (tm.Month < 10)
    {
      lcd.setCursor(first_date_cursor + 3, row_date); // column = first_date_cursor + 3, row = row_date
      lcd.print('0'); // month [1..12]
      lcd.setCursor(first_date_cursor + 4, row_date); // column = first_date_cursor + 4, row = row_date
      lcd.print(tm.Month); // month [1..12]
    } else
    {
      lcd.setCursor(first_date_cursor + 3, row_date); // column = first_date_cursor + 3, row = row_date
      lcd.print(tm.Month); // month [1..12]
    }

    // year [xxxx]
    lcd.setCursor(first_date_cursor + 6, row_date); // column = first_date_cursor + 6, row = row_date
    lcd.print(tmYearToCalendar(tm.Year)); // year [xxxx]

    // print clock to LCD
    // hour [0..23]
    if (tm.Hour < 10) // hour of RTC < 10
    {
      lcd.setCursor(first_clock_cursor, row_clock); // column = first_clock_cursor, row = row_clock
      lcd.print('0'); // hour [0..23]
      lcd.setCursor(first_clock_cursor + 1, row_clock); // column = first_clock_cursor + 1, row = row_clock
      lcd.print(tm.Hour); // hour [0..23]
    } else // hour of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor, row_clock); // column = first_clock_cursor, row = row_clock
      lcd.print(tm.Hour); // hour [0..23]
    }

    // minute [0..59]
    if (tm.Minute < 10) // minute of RTC < 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock); // column = first_clock_cursor + 3, row 3 = row_clock
      lcd.print('0'); // minute [0..59]
      lcd.setCursor(first_clock_cursor + 4, row_clock); // column = first_clock_cursor + 4, row 3 = row_clock
      lcd.print(tm.Minute); // minute [0..59]
    } else // minute of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor + 3, row_clock); // column = first_clock_cursor + 3, row = row_clock
      lcd.print(tm.Minute); // minute [0..59]
    }

    // second [0..59]
    if (tm.Second < 10) // second of RTC < 10
    {
      lcd.setCursor(first_clock_cursor + 6, row_clock); // column = first_clock_cursor + 6, row = row_clock
      lcd.print('0'); // second [0..59]
      lcd.setCursor(first_clock_cursor + 7, row_clock); // column = first_clock_cursor + 7, row = row_clock
      lcd.print(tm.Second); // second [0..59]
    } else // second of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor + 6, row_clock); // column = first_clock_cursor + 6, row = row_clock
      lcd.print(tm.Second); // second [0..59]
    }
  } else {
    if (RTC.chipPresent()) {
      lcd.setCursor(first_date_cursor, row_date); // column 4, row = row_date
      lcd.print("ERROR_SetTime");
      lcd.setCursor(first_clock_cursor, row_clock); // column 4, row = row_clock
      lcd.print("ERROR_SetTime");
    } else {
      lcd.setCursor(first_date_cursor, row_date); // column 4, row = row_date
      lcd.print("ERROR_Circuit");
      lcd.setCursor(first_clock_cursor, row_clock); // column 4, row = row_clock
      lcd.print("ERROR_Circuit");
    }
  }
}
/*---------------------------------------------------------------------------------*/
