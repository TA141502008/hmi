/*******************************************************************
====================================================================
Project        : Experiment
                 Mixing LCD 16x4 and Keypad 4x4
Version        : 1.0
Date Created   : August 27, 2015
Author         : Baharuddin Aziz (http://baha.web.id)
Company        : Department of Electrical Engineering
                 School of Electrical Engineering and Informatics
                 Bandung Institute of Technology (ITB)
Summary        : This program for implement 4x4 Matrix Keypad
                 to LCD 16x4 that connected to Arduino board.
                 Every keypress will be display on LCD 16x4.
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
> Thanks to :
    1. Alexander Brevig (alexanderbrevig@gmail.com) for providing :
        - CustomKeypad source code example
    2. David A. Mellis (Arduino Team) for providing :
        - LiquidCrystal source code example
    3. Tom Igoe (Arduino Team) for providing :
        - LiquidCrystal source code example
    4. Limo Fried for providing :
        - LiquidCrystal source code example
====================================================================
*******************************************************************/

/*------------------( Import needed libraries )--------------------*/
#include <LiquidCrystal.h> // include the LCD library code
#include <Keypad.h> // include the 4x4 matrix keypad library code
/*-----------------------------------------------------------------*/

/*---------------------( Declare Constants and Pin Numbers )-----------------------*/
// LCD 16x4
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

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
/*--------------------------------------------------------------------------------*/

/*-----( Declare objects )-----*/
/*-----------------------------*/

/*-------------------------( Declare Variables )-------------------------*/
int hitung; // counter in seconds
int i; // counter for keypress in line 3
int j; // dummy variable for display keypress counter in serial monitor
/*-----------------------------------------------------------------------*/

/*--------------------( SETUP )-----------------------*/
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // KEYPAD
  Serial.begin(9600);

  // LCD 16x4
  // set up the LCD's number of columns and rows
  lcd.begin(16, 4);

  // print a message to the LCD
  // default line 1
  lcd.print("HMI v0.1 Testing");

  // line 2
  lcd.setCursor(0, 1);
  lcd.print("Silakan ketik :");

  // initialize the variable
  hitung = 0;
  i = 0;
  j = 1;
  //  func_keypad();

}
/*----------------------------------------------------*/

/*----------------( MAIN LOOP )-------------------*/

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  char keypressed = myKeypad.getKey();

  // LCD 16x4
  func_lcd();

  // KEYPAD
  /* If key is pressed, this key is stored in 'keypressed' variable.
   If key is not equal to 'NO_KEY', then this key is printed out.
   If count = 17, then count is reset back to 0
   (this means no key is pressed during the whole keypad scan process).
  */
  if ((keypressed != NO_KEY) && (i <= 15))
  {
    lcd.setCursor(i, 2); // line 3 of LCD
    lcd.print(keypressed);
    i++;
    j++;
  }
  else if ((keypressed != NO_KEY) && (i > 15))
  {
    i = 0;
    lcd.setCursor(i, 2); // line 3 of LCD
    lcd.print(keypressed);
    i++;
    j++;
  }

  if (i < j)
  {
    Serial.println(i);
  }
}

/* --------------------------------------------------*/

/*--------------( PROCEDURE DETAIL )-------------------*/
void func_lcd()
{
  // LCD 16x4
  // set the cursor to column 0, line 4
  // note: line 4 is the third row
  lcd.setCursor(0, 3);
  lcd.print("Hitung = ");
  hitung = millis() / 1000;
  lcd.print(hitung);
}
/*-----------------------------------------------------*/
