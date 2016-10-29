/****************************************************************************
=============================================================================
Project        : Experiment
                 Mixing LCD 16x4, Keypad 4x4, RTC, and SD Card Module
Version        : 1.2
Date Created   : August 27, 2015
Date Modified  : September 9, 2015
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
****************************************************************************/

/*------------------------( Import needed libraries )-----------------------------*/
#include <LiquidCrystal.h> // include the LCD library code
#include <Keypad.h> // include the 4x4 matrix keypad library code
#include <Wire.h>
#include <Time.h> // include time library code
#include <DS1307RTC.h> // include the DS1307 RTC library code
#include <SPI.h>
#include <SD.h> // include the SD library
/*---------------------------------------------------------------------------------*/

/*---------------------( Declare Constants and Pin Numbers )-----------------------*/
/* LCD 16x4 */
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// constant for first keypress
const int first_keypress = 6; // first column for keypress cursor
const int first_date_cursor = 4; // first column for date cursor
const int first_clock_cursor = 4; // first column for clock cursor
const int row_title = 0; // row that used for title
const int row_keypress = 3; // row that used for keypress
const int row_date = 1; // row that used to display the date
const int row_clock = 2; // row that used to display the clock
const int column_title_counter = 11; // first column for title counter
const int length_of_LCD_keypress_value = 16 - first_keypress; // length of LCD keypress value

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

/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare objects )---------------------------------*/
/*---------------------------------------------------------------------------------*/

/*-----------------------------( Declare Variables )-------------------------------*/
/* LCD and Keypad */
char LCD_keypress_value[length_of_LCD_keypress_value]; // value of keypress
int LCD_keypress_value_index; // index of LCD_keypress_value
int LCD_i; // counter for keypress in line 2
int LCD_j; // dummy variable for display keypress counter in serial monitor

/* RTC */
tmElements_t tm;
int RTC_ThisDay;
int RTC_NextDay;

/* SD Card Module */
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// file name for data logging
char SDCard_FileName[12];

/*---------------------------------------------------------------------------------*/

/*-----------------------------------( SETUP )-------------------------------------*/
void setup()
{
  /* Initialize the variables */
  procedure_Clear_LCD_keypress_value(); // reset or fill blank to LCD_keypress_value (replace with '_')
  LCD_keypress_value_index = 1;
  LCD_i = first_keypress;
  LCD_j = 1;

  /* Serial Monitor */
  Serial.begin(9600);

  /* LCD 16x4 */
  // set up the LCD's number of columns and rows
  lcd.begin(16, 4); // sum of column = 16, sum of row = 4

  // print title to the LCD
  lcd.print("HMI_v0.1_|_"); // default at row 1

  // print label to the LCD
  // keypress label
  lcd.setCursor(0, row_keypress); // column 0, row = row_keypress
  lcd.print("Ketik");
  lcd.setCursor(first_keypress, row_keypress); // column = first_keypress, row = row_keypress
  procedure_ClearScreen_keypress(); // clear the screen of keypress label

  // date
  lcd.setCursor(0, row_date); // column 0, row = row_date
  lcd.print("Tgl");
  lcd.setCursor(first_date_cursor + 2, row_date); // column = first_date_cursor + 2, row = row_date
  lcd.print("-");
  lcd.setCursor(first_date_cursor + 5, row_date); // column = first_date_cursor + 5, row = row_date
  lcd.print("-");

  // clock
  lcd.setCursor(0, row_clock); // column 0, row = row_clock
  lcd.print("Jam");
  lcd.setCursor(first_date_cursor + 2, row_clock); // column = first_date_cursor + 2, row = row_clock
  lcd.print(":");
  lcd.setCursor(first_date_cursor + 5, row_clock); // column = first_date_cursor + 5, row = row_clock
  lcd.print(":");

  /* RTC */
  RTC_ThisDay = tm.Day;
  RTC_NextDay = RTC_ThisDay + 1;

  /* SD CARD MODULE */
  procedure_SDCard_Initializing();
  //  procedure_SDCard_TEST_all(); // for TESTING only
}
/*---------------------------------------------------------------------------------*/

/*--------------------------------( MAIN LOOP )------------------------------------*/
void loop()
{
  char keypressed = myKeypad.getKey();

  // counter at title of LCD
  procedure_LCD_counter();

  // print date and clock to LCD
  procedure_RTC();

  /* REMOVE OLD FILE in SD CARD */
  RTC_ThisDay = tm.Day; // reset RTC_ThisDay variable with date of today
  if (RTC_ThisDay == RTC_NextDay)
  {
    procedure_SDCard_RemoveOldFile();
    RTC_NextDay += 1;
  }

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

  /* SD CARD MODULE */
  if (tm.Second % 2 == 0)
  {
    procedure_SDCard_FileName();
    procedure_SDCard_KeypressLogging();
    delay(1000);
  }
}
/*---------------------------------------------------------------------------------*/

/*-----------------------------( PROCEDURE DETAIL )--------------------------------*/
void procedure_LCD_counter()
// input   : N/A
// output  : counter in integer
// process : counting every 1 second
{
  int hitung;

  lcd.setCursor(column_title_counter, row_title); // column = first_title_cursor, row = row_title

  hitung = 0; // initialization
  hitung = millis() / 1000;
  lcd.print(hitung); // print counter on LCD
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

    // Second [0..59]
    if (tm.Second < 10) // Second of RTC < 10
    {
      lcd.setCursor(first_clock_cursor + 6, row_clock); // column = first_clock_cursor + 6, row = row_clock
      lcd.print('0'); // Second [0..59]
      lcd.setCursor(first_clock_cursor + 7, row_clock); // column = first_clock_cursor + 7, row = row_clock
      lcd.print(tm.Second); // Second [0..59]
    } else // Second of RTC >= 10
    {
      lcd.setCursor(first_clock_cursor + 6, row_clock); // column = first_clock_cursor + 6, row = row_clock
      lcd.print(tm.Second); // Second [0..59]
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

void procedure_SDCard_Initializing()
// input   : N/A
// output  : status of SD card
// process : check the present of SD card
{
  /* Initializing #2 */
  Serial.println("#2");
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
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
    }
    else {
      Serial.print("Deleting ");
      Serial.print(FileName);
      Serial.println(" COMPLETED!");
    }
  }
  else {
    Serial.println("No file need to be deleted!");
  }
}

void procedure_SDCard_KeypressLogging()
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

    /* keypress */
    for (int i = 1; i <= length_of_LCD_keypress_value; i++)
    {
      myFile1.print(LCD_keypress_value[i]);
    }
    myFile1.println("");

    // close the file
    myFile1.close();

    Serial.println("done."); // display on serial monitor (Arduino IDE)

  } else {
    // if the file didn't open, print an error
    Serial.print("ERROR opening "); // display on serial monitor (Arduino IDE)
    Serial.print(SDCard_FileName);
    Serial.println("!");
  }
}

void procedure_SDCard_TEST_all() // TEST - not combined with others
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

  if (!SD.begin(53)) {
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

