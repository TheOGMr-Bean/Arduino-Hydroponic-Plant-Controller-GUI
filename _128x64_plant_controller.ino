#include <SPI.h>
#include <Wire.h>
#include "ds3231.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


unsigned long prev, interval = 100;
byte flash = 0;

byte lHourOn = 0;
byte lMinOn = 00;
byte lHourOff = 23;
byte lMinOff = 59;
boolean poweredOn = 0;
boolean poweredOn1 = 0;
byte validStart = 0;
byte validEnd = 0;
byte pMinOn  = 0;
byte pMinOff = 0;
byte pHourOff = 0;
byte hours = 0;
byte minutes = 1;

int mode = 0;
int lastCount = 0;
int lampControlSet = 0;
byte lSet = 1;
int pumpControlSet = 0;
byte pSet = 1;
byte tempSet;
uint8_t secset = 0;
uint8_t minset = 1;
uint8_t hourset = 2;
uint8_t wdayset = 3;
uint8_t mdayset = 4;
uint8_t monset = 5;

byte previous = 0;
byte previousMin = 0;
int pInterval = pMinOff;
boolean pumpState = false;

// Encoder Pins

const byte                                 lightPin   = 6;
const byte                                 pumpPin   = 7;
const byte                                 PinCLK   = 2;
const byte                                 PinDT    = 3;
const byte                                 PinSW    = 4;

volatile int                              virtualPosition = 0;

// Interrupt service routine
void isr ()  {

  static unsigned long  lastInterruptTime = 0;

  unsigned long   interruptTime = millis();

  if (interruptTime - lastInterruptTime > 120) {
    if (!digitalRead(PinDT))
      virtualPosition = virtualPosition + 1;
    else
      virtualPosition = virtualPosition - 1;
  }
  lastInterruptTime = interruptTime;
} // ISR

//----------------------------------------------------------------------------------SETUP

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(0x00);

  pinMode(lightPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(PinCLK, INPUT);
  pinMode(PinDT, INPUT);
  pinMode(PinSW, INPUT_PULLUP);

  digitalWrite(6, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(A0, HIGH);


  attachInterrupt(0, isr, FALLING);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

//----------------------------------------------------------------------------------LOOP

void loop() {
  digitalWrite(7, pumpState);
  unsigned long now = millis();
  struct ts t;
  DS3231_get(&t);
  uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };
  byte currentMin = t.min;
  byte current = t.hour;
  if ((now - prev > interval)) {
    if (flash == 0) {
      flash = 1;
    } else {
      flash = 0;
    }
    //----------------
    if ((lSet) == 3) {
      if (t.sec == 0) {
        if (t.hour >= lHourOn && t.min >= lMinOn && t.hour < lHourOff && t.min < lMinOff) {
          Serial.print("passed");
          validStart = 1;
          validEnd = 0;
        }
        else {
          validStart = 0;
          validEnd = 1;
        }
      }
    }
    
    //----------------
    
    //start appropriate pump timer based on hours/minutes selection.
    if (hours == 1) {
      pumpHours();
    }
    if (minutes == 1) {
      pumpMinutes();
    }

    //----------------
    display.clearDisplay(); //Clear display buffer from last refresh
    //LIGHT ON/OFF/AUTO MAIN PAGE DISPLAY
    if (mode <= 2) {
      display.drawCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
      display.drawRect(24, 17, 5, 11, WHITE);
      display.drawLine(26, 28, 27, 28, WHITE);
      display.drawLine(23, 21, 29, 23, WHITE);
      display.drawLine(23, 23, 29, 25, WHITE);
      display.drawLine(23, 25, 27, 26, WHITE);
      if ((lSet) == 1) {
        digitalWrite(6, LOW);
      }
      if ((lSet) == 2) { //Display alarm on indicator if alarm turned on
        display.fillCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
        for ( int z = 0; z < 360; z = z + 30 ) {
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (26 + (sin(angle) * 9));
          int y2 = (16 - (cos(angle) * 9));
          int x3 = (26 + (sin(angle) * (9 + 3)));
          int y3 = (16 - (cos(angle) * (9 + 3)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }
        digitalWrite(6, HIGH);
      }
      if ((lSet) == 3) {
        display.setCursor(50, 14);
        display.setTextSize(1);
        display.print("A");
        for ( int z = 0; z < 360; z = z + 45 ) {
          //Begin at 0° and stop at 360°
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (52 + (sin(angle) * 6));
          int y2 = (18 - (cos(angle) * 6));
          int x3 = (52 + (sin(angle) * (6 + 2)));
          int y3 = (18 - (cos(angle) * (6 + 2)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }

        if ((validStart) == 1) {
          display.fillCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
          for ( int z = 0; z < 360; z = z + 30 ) {
            float angle = z ;
            angle = (angle / 57.29577951) ;
            int x2 = (26 + (sin(angle) * 9));
            int y2 = (16 - (cos(angle) * 9));
            int x3 = (26 + (sin(angle) * (9 + 3)));
            int y3 = (16 - (cos(angle) * (9 + 3)));
            display.drawLine(x2, y2, x3, y3, WHITE);
          }
          Serial.println("turn lights on");
          digitalWrite(6, HIGH);
        }
        else {
          Serial.println("turn lights off");
          digitalWrite(6, LOW);
        }
      }
    }

    //----------------

    if (mode <= 2) {
      //PUMP ON/OFF/AUTO MAIN PAGE DISPLAY
      if ((pSet) == 1) { //Set the pump variable to off (we will use 1).
        previous = 0;
        previousMin = 0;
        pInterval = pMinOff;
        pumpState = false;
        digitalWrite(7, LOW);
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
      }
      if ((pSet) == 2) { //Set the pump variable to on (we will use 2).
        previous = 0;
        previousMin = 0;
        pInterval = pMinOff;
        pumpState = false;
        digitalWrite(7, HIGH);
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
        if (flash) {
          display.setTextSize(2);
          display.setCursor(22, 38); //Position text cursor for pump off indicator
          display.setTextColor(BLACK);
          display.print("P");
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.drawRect(20, 36, 14, 18, WHITE);
        }
      }
      if ((pSet) == 3) { //Set the pump variable to auto (we will use 3).
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
        display.setCursor(50, 40); //Position text cursor for alarm on indicator
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("A");
        for ( int z = 0; z < 360; z = z + 45 ) {
          //Begin at 0° and stop at 360°
          float angle = z ;
          angle = (angle / 57.29577951) ; //Convert degrees to radians
          int x2 = (52 + (sin(angle) * 6));
          int y2 = (44 - (cos(angle) * 6));
          int x3 = (52 + (sin(angle) * (6 + 2)));
          int y3 = (44 - (cos(angle) * (6 + 2)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }
        if ((poweredOn1) == 1) {
          Serial.println(t.hour);
          Serial.println("  ");
          Serial.println(t.min);
          Serial.println("turn pump on");
          if (flash) {
            display.fillRect(20, 36, 14, 18, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(2);
            display.setCursor(22, 38); //Position text cursor for pump on indicator
            display.print("P");
            display.setTextColor(WHITE);
            display.setTextSize(1);
          }
          //  }
        }
        else {
          Serial.println(t.hour);
          Serial.println("  ");
          Serial.println(t.min);
          Serial.println("turn pump off");

        }
      }
    }

    //----------------

    if (mode <= 2) {
      if ((now - prev > interval)) {

        //DISPLAY CLOCK

        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 22, WHITE);
        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 13, BLACK);

        for ( int z = 0; z < 360; z = z + 30 ) {
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (89 + (sin(angle) * 18));
          int y2 = (30 - (cos(angle) * 18));
          int x3 = (89 + (sin(angle) * (18 + 6)));
          int y3 = (30 - (cos(angle) * (18 + 6)));
          display.drawLine(x2, y2, x3, y3, BLACK);
        }

        display.setTextColor(WHITE, BLACK);
        display.setCursor(94, 27);
        printDay(t.wday);

        display.setTextColor(BLACK, WHITE);
        display.setCursor(83, 9);
        display.print("12");
        display.setCursor(87, 45);
        display.print("6");
        display.setCursor(69, 28);
        display.print("9");

        //Position and display second hand
        float angle = t.sec * 6 ;
        angle = (angle / 57.29577951) ;
        int x3 = (89 + (sin(angle) * (20)));
        int y3 = (30 - (cos(angle) * (20)));
        display.drawLine(89, 30, x3, y3, BLACK);

        angle = t.sec * 6 ;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (13)));
        y3 = (30 - (cos(angle) * (13)));
        display.drawLine(89, 30, x3, y3, WHITE);

        //Position and display minute hand
        angle = t.min * 6;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (20 - 2)));
        y3 = (30 - (cos(angle) * (20 - 2)));
        display.drawLine(89, 30, x3, y3, BLACK);

        angle = t.min * 6;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (13)));
        y3 = (30 - (cos(angle) * (13)));
        display.drawLine(89, 30, x3, y3, WHITE);

        //Position and display hour hand
        angle = t.hour * 30 + int((t.min / 12) * 6);
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (12)));
        y3 = (30 - (cos(angle) * (12)));
        display.drawLine(89, 30, x3, y3, WHITE);

        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 3, BLACK);
      }
      if (t.hour > 11) {
        display.drawRect(114, 26, 13, 9, WHITE);
        display.setTextColor(BLACK, WHITE);
        display.setCursor(115, 27);
        display.print("PM");
      }
      display.setTextColor(WHITE, BLACK);
    }


    //---------------------------------------------------------------------------MAIN MENU

    if (mode > 2 && mode < 7) {
      display.setTextSize(1);
      display.drawLine(10, 23, 118, 23, WHITE);
      display.setCursor(40, 10);
      display.print("SETTINGS");

      //light menu
      display.drawCircle(display.width() / 6 + 5, display.height() / 2 + 10, 6, WHITE);
      display.drawRect(24, 43, 5, 11, WHITE);
      display.drawLine(25, 54, 27, 54, WHITE);
      display.drawLine(23, 47, 29, 49, WHITE);
      display.drawLine(23, 49, 29, 51, WHITE);
      display.drawLine(23, 51, 28, 52, WHITE);
      display.drawRect(54, 36, 19, 20, WHITE);

      //pump menu
      display.setTextSize(2);
      display.setCursor(58, 39);
      display.print("P");
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(68, 45);
      display.print(">>");
      display.setTextColor(WHITE);

      //time menu
      display.setTextSize(2);
      display.setCursor(87, 39);
      display.print("T");
      display.setTextSize(1);
      display.setCursor(94, 46);
      display.print("IME");
    }

    //----------------------------------------------------------------------LIGHT TIMER MENU
    //CONVERT 24-HOUR TO 12-HOUR, DISPLAY LIGHT TIMER MENU
    if (mode > 6 && mode < 14) {
      display.drawLine(64, 5, 64, 40, WHITE);
      display.drawLine(64, 5, 64, 40, WHITE);
      display.setTextSize(3);
      display.setCursor(1, 16);

      if (lHourOn == 0) {
        display.print("12");
      }
      else if (lHourOn < 13 && lHourOn >= 10) {
        display.print(lHourOn);
      }
      else if (lHourOn < 10) {
        display.print(" ");
        display.print(lHourOn);
      }
      else if (lHourOn >= 13 && lHourOn >= 22) {
        display.print(lHourOn - 12);
      }
      else {
        display.print(" ");
        display.print(lHourOn - 12);
      }
      //--------
      display.setTextSize(2);
      display.setCursor(32, 28);
      display.print(":");
      //--------
      display.setCursor(39, 24);
      if (lMinOn < 10) {
        display.print("0");
      }
      display.print(lMinOn);
      //--------
      display.setTextSize(1);
      display.setCursor(44, 15);
      if (lHourOn < 12) {
        display.print(" AM");
      }
      else {
        display.print(" PM");
      }
      //--------

      display.setCursor(66, 16);
      display.setTextSize(3);
      if (lHourOff == 0) {
        display.print("12");
      }
      else if (lHourOff < 13 && lHourOff >= 10) {
        display.print(lHourOff);
      }
      else if (lHourOff < 10) {
        display.print(" ");
        display.print(lHourOff);
      }
      else if (lHourOff >= 13 && lHourOff >= 22) {
        display.print(lHourOff - 12);
      }
      else {
        display.print(" ");
        display.print(lHourOff - 12);
      }
      //--------
      display.setTextSize(2);
      display.setCursor(97, 28);
      display.print(":");
      //--------
      display.setCursor(104, 24);
      if (lMinOff < 10) {
        display.print("0");
      }
      display.print(lMinOff);
      //--------
      display.setTextSize(1);
      display.setCursor(109, 15);
      if (lHourOff < 12) {
        display.print(" AM");
      }
      else {
        display.print(" PM");
      }
      //--------

      display.setTextSize(1);
      display.setCursor(5, 50);
      display.print("<<");
    }

    //----------------------------------------------------------------------PUMP TIMER MENU
    //DISPLAY PUMP TIMER MENU
    if (mode > 13 && mode < 21) {

      display.setTextSize(3);
      display.setCursor(30, 17);
      //--------
      if (pMinOn < 10) {
        display.print("0");
      }
      display.print(pMinOn);
      //--------
      if (hours == 1) {
        display.setTextSize(2);
        display.setCursor(68, 24);
        if (pHourOff < 10) {
          display.print("0");
        }
        display.print(pHourOff);
      }
      //--------
      if (minutes == 1) {
        display.setTextSize(2);
        display.setCursor(68, 24);
        if (pMinOff < 10) {
          display.print("0");
        }
        display.print(pMinOff);
      }
      //--------
      if (hours == 1) {
        display.drawRect(106, 14, 9, 11, WHITE);
      }
      display.setTextSize(1);
      display.setCursor(108, 16);
      display.print("H");
      //--------
      if (minutes == 1) {
        display.drawRect(106, 28, 9, 11, WHITE);
      }
      display.setTextSize(1);
      display.setCursor(108, 30);
      display.print("M");
      //--------
      display.setTextSize(1);
      display.setCursor(5, 50);
      display.print("<<");
    }

    //----------------------------------------------------------------------------SET TIME
    if (mode > 21 && mode < 25) {
      display.drawCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
      display.drawRect(24, 17, 5, 11, WHITE);
      display.drawLine(26, 28, 27, 28, WHITE);
      display.drawLine(23, 21, 29, 23, WHITE);
      display.drawLine(23, 23, 29, 25, WHITE);
      display.drawLine(23, 25, 27, 26, WHITE);
      if ((lSet) == 1) {
        digitalWrite(6, LOW);
      }
      if ((lSet) == 2) { //Display alarm on indicator if alarm turned on
        display.fillCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
        for ( int z = 0; z < 360; z = z + 30 ) {
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (26 + (sin(angle) * 9));
          int y2 = (16 - (cos(angle) * 9));
          int x3 = (26 + (sin(angle) * (9 + 3)));
          int y3 = (16 - (cos(angle) * (9 + 3)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }
        digitalWrite(6, HIGH);
      }
      if ((lSet) == 3) {
        display.setCursor(50, 14);
        display.setTextSize(1);
        display.print("A");
        for ( int z = 0; z < 360; z = z + 45 ) {
          //Begin at 0° and stop at 360°
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (52 + (sin(angle) * 6));
          int y2 = (18 - (cos(angle) * 6));
          int x3 = (52 + (sin(angle) * (6 + 2)));
          int y3 = (18 - (cos(angle) * (6 + 2)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }

        if ((validStart) == 1) {
          display.fillCircle(display.width() / 6 + 5, display.height() / 4, 6, WHITE);
          for ( int z = 0; z < 360; z = z + 30 ) {
            float angle = z ;
            angle = (angle / 57.29577951) ;
            int x2 = (26 + (sin(angle) * 9));
            int y2 = (16 - (cos(angle) * 9));
            int x3 = (26 + (sin(angle) * (9 + 3)));
            int y3 = (16 - (cos(angle) * (9 + 3)));
            display.drawLine(x2, y2, x3, y3, WHITE);
          }
          Serial.println("turn lights on");
          digitalWrite(6, HIGH);
        }
        else {
          Serial.println("turn lights off");
          digitalWrite(6, LOW);
        }
      }
    }

    //----------------

    if (mode > 21 && mode < 25) {
      //PUMP ON/OFF/AUTO MAIN PAGE DISPLAY
      if ((pSet) == 1) { //Set the pump variable to off (we will use 1).
        previous = 0;
        previousMin = 0;
        pInterval = pMinOff;
        pumpState = false;
        digitalWrite(7, LOW);
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
      }
      if ((pSet) == 2) { //Set the pump variable to on (we will use 2).
        previous = 0;
        previousMin = 0;
        pInterval = pMinOff;
        pumpState = false;
        digitalWrite(7, HIGH);
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
        if (flash) {
          display.setTextSize(2);
          display.setCursor(22, 38); //Position text cursor for pump off indicator
          display.setTextColor(BLACK);
          display.print("P");
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.drawRect(20, 36, 14, 18, WHITE);
        }
      }
      if ((pSet) == 3) { //Set the pump variable to auto (we will use 3).
        display.setTextSize(2);
        display.setCursor(22, 38); //Position text cursor for pump off indicator
        display.print("P");
        display.setTextSize(1);
        display.drawRect(20, 36, 14, 18, WHITE);
        display.setCursor(50, 40); //Position text cursor for alarm on indicator
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("A");
        for ( int z = 0; z < 360; z = z + 45 ) {
          //Begin at 0° and stop at 360°
          float angle = z ;
          angle = (angle / 57.29577951) ; //Convert degrees to radians
          int x2 = (52 + (sin(angle) * 6));
          int y2 = (44 - (cos(angle) * 6));
          int x3 = (52 + (sin(angle) * (6 + 2)));
          int y3 = (44 - (cos(angle) * (6 + 2)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }
        if ((poweredOn1) == 1) {
          Serial.println(t.hour);
          Serial.println("  ");
          Serial.println(t.min);
          Serial.println("turn pump on");
          if (flash) {
            display.fillRect(20, 36, 14, 18, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(2);
            display.setCursor(22, 38); //Position text cursor for pump on indicator
            display.print("P");
            display.setTextColor(WHITE);
            display.setTextSize(1);
          }
          //  }
        }
        else {
          Serial.println(t.hour);
          Serial.println("  ");
          Serial.println(t.min);
          Serial.println("turn pump off");

        }
      }
    }

    if (mode > 21 && mode < 25) {
      if ((now - prev > interval)) {

        //DISPLAY CLOCK

        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 22, WHITE);
        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 13, BLACK);

        for ( int z = 0; z < 360; z = z + 30 ) {
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (89 + (sin(angle) * 18));
          int y2 = (30 - (cos(angle) * 18));
          int x3 = (89 + (sin(angle) * (18 + 6)));
          int y3 = (30 - (cos(angle) * (18 + 6)));
          display.drawLine(x2, y2, x3, y3, BLACK);
        }

        display.setTextColor(WHITE, BLACK);
        display.setCursor(94, 27);
        printDay(t.wday);

        display.setTextColor(BLACK, WHITE);
        display.setCursor(83, 9);
        display.print("12");
        display.setCursor(87, 45);
        display.print("6");
        display.setCursor(69, 28);
        display.print("9");

        //Position and display second hand
        float angle = t.sec * 6 ;
        angle = (angle / 57.29577951) ;
        int x3 = (89 + (sin(angle) * (20)));
        int y3 = (30 - (cos(angle) * (20)));
        display.drawLine(89, 30, x3, y3, BLACK);

        angle = t.sec * 6 ;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (13)));
        y3 = (30 - (cos(angle) * (13)));
        display.drawLine(89, 30, x3, y3, WHITE);

        //Position and display minute hand
        angle = t.min * 6;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (20 - 2)));
        y3 = (30 - (cos(angle) * (20 - 2)));
        display.drawLine(89, 30, x3, y3, BLACK);

        angle = t.min * 6;
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (13)));
        y3 = (30 - (cos(angle) * (13)));
        display.drawLine(89, 30, x3, y3, WHITE);

        //Position and display hour hand
        angle = t.hour * 30 + int((t.min / 12) * 6);
        angle = (angle / 57.29577951) ;
        x3 = (89 + (sin(angle) * (12)));
        y3 = (30 - (cos(angle) * (12)));
        display.drawLine(89, 30, x3, y3, WHITE);

        display.fillCircle(display.width() / 2 + 25, display.height() / 2 - 2, 3, BLACK);
      }
      if (t.hour > 11) {
        display.drawRect(114, 26, 13, 9, WHITE);
        display.setTextColor(BLACK, WHITE);
        display.setCursor(115, 27);
        display.print("PM");
      }
      display.setTextColor(WHITE, BLACK);
    }

    //-----------------------------------------------------------------BEGIN MENU SELECTION (MODE)
    switch (mode)
    {
      case 0:
        break;

      //MANUAL LIGHT CONTROL
      case 1:
        display.drawCircle(display.width() / 6 + 5, display.height() / 4, 9, WHITE);
        if (digitalRead(4) == LOW) {
          lampControlSet = lSet;
          lampControlSet = lampControlSet + 1;
          if (lampControlSet > 3) {
            lampControlSet = 1;
          }
          lSet = lampControlSet;
          while (digitalRead(4) == LOW) {}
        }
        break;

      //MANUAL PUMP CONTROL
      case 2:
        display.drawRect(18, 34, 18, 22, WHITE);
        if (digitalRead(4) == LOW) {
          pumpControlSet = pSet;
          pumpControlSet = pumpControlSet + 1;
          if (pumpControlSet > 3) {
            pumpControlSet = 1;
          }
          pSet = pumpControlSet;
          while (digitalRead(4) == LOW) {}

        }
        break;

      //LIGHT SETTINGS
      case 3:
        for ( int z = 0; z < 360; z = z + 30 ) {
          float angle = z ;
          angle = (angle / 57.29577951) ;
          int x2 = (26 + (sin(angle) * 9));
          int y2 = (42 - (cos(angle) * 9));
          int x3 = (26 + (sin(angle) * (9 + 3)));
          int y3 = (42 - (cos(angle) * (9 + 3)));
          display.drawLine(x2, y2, x3, y3, WHITE);
        }

        if (flash) {
          display.fillCircle(display.width() / 6 + 5, display.height() / 2 + 10, 6, WHITE);
        }

        if (digitalRead(4) == LOW) {
          mode = 8;
          while (digitalRead(4) == LOW) {}
        }
        break;

      //PUMP SETTINGS
      case 4:
        if (flash) {
          display.fillRect(68, 45, 15, 10, BLACK);
          display.setTextSize(1);
          display.drawLine(72, 45, 72, 55, WHITE);
          display.setCursor(44, 44);
          display.setTextColor(WHITE, BLACK);
          display.print(">>");
        }
        if (digitalRead(4) == LOW) {
          mode = 15;
          while (digitalRead(4) == LOW) {}
        }
        break;

      //TIME/DATE SETTINGS
      case 5:
        display.fillRect(86, 30, 26, 26, BLACK);
        if ((now - prev > interval)) {
          for ( int z = 0; z < 360; z = z + 90 ) {
            //Begin at 0° and stop at 360°
            float angle = z ;
            angle = (angle / 57.29577951) ;
            int x2 = (99 + (sin(angle) * 13));
            int y2 = (43 - (cos(angle) * 13));
            int x3 = (99 + (sin(angle) * (13 - 2)));
            int y3 = (43 - (cos(angle) * (13 - 2)));
            display.drawLine(x2, y2, x3, y3, WHITE);
          }
          //Position and display second hand
          float angle = t.sec * 6 ;
          angle = (angle / 57.29577951) ;
          int x3 = (99 + (sin(angle) * (10 + 1)));
          int y3 = (43 - (cos(angle) * (10 + 1)));
          display.drawLine(99, 43, x3, y3, WHITE);

          //Position and display minute hand
          angle = t.min * 6;
          angle = (angle / 57.29577951) ;
          x3 = (99 + (sin(angle) * (10 - 1)));
          y3 = (43 - (cos(angle) * (10 - 1)));
          display.drawLine(99, 43, x3, y3, WHITE);

          //Position and display hour hand
          angle = t.hour * 30 + int((t.min / 12) * 6);
          angle = (angle / 57.29577951) ;
          x3 = (99 + (sin(angle) * (10 - 3)));
          y3 = (43 - (cos(angle) * (10 - 3)));
          display.drawLine(99, 43, x3, y3, WHITE);
        }

        display.fillCircle(display.width() - 29, display.height() / 2 + 11, 2, BLACK);
        if (flash) {
          display.drawCircle(display.width() - 29, display.height() / 2 + 11, 2, WHITE);
        }

        if (digitalRead(4) == LOW) {
          mode = 22;
          while (digitalRead(4) == LOW) {}
        }
        break;
      case 6:
        mode = 5;
        break;

      //-------------------------------------------------------------------------------------------------------------------------OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

      //LIGHT TIMER HOUR (ON) SETTING
      case 7:
        mode = 12;
        break;
      case 8:
        display.setCursor(13, 5);
        display.print("ON TIME");

        display.drawRect(0, 14, 36, 26, WHITE);
        if (digitalRead(4) == LOW) {
          delay(20);
          tempSet = lHourOn;
          tempSet = tempSet + 1;
          if (tempSet > 23) {
            tempSet = 0;
          }
          lHourOn = tempSet;
          delay(20);
        }
        Serial.print(lHourOn);
        break;

      //LIGHT TIMER MINUTE (ON) SETTING
      case 9:
        display.setCursor(13, 5);
        display.print("ON TIME");

        display.drawRect(37, 22, 26, 18, WHITE);
        if (digitalRead(4) == LOW) {
          tempSet = lMinOn;
          tempSet = tempSet + 1;
          if (tempSet > 59) {
            tempSet = 0;
          }
          lMinOn = tempSet;
        }
        Serial.print(lMinOn);
        break;

      //LIGHT TIMER HOUR (OFF) SETTING
      case 10:
        display.setCursor(75, 5);
        display.print("OFF TIME");
        display.drawRect(67, 14, 34, 25, WHITE);
        if (digitalRead(4) == LOW) {
          delay(20);
          tempSet = lHourOff;
          tempSet = tempSet + 1;
          if (tempSet > 23) {
            tempSet = 0;
          }
          lHourOff = tempSet;
          delay(20);
        }
        Serial.print(lHourOff);
        break;

      //LIGHT TIMER MINUTE (OFF) SETTING
      case 11:

        display.setCursor(75, 5);
        display.print("OFF TIME");

        display.drawRect(102, 22, 26, 18, WHITE);
        if (digitalRead(4) == LOW) {
          tempSet = lMinOff;
          tempSet = tempSet + 1;
          if (tempSet > 59) {
            tempSet = 0;
          }
          lMinOff = tempSet;
        }
        Serial.print(lMinOff);
        break;
      case 12:
        display.drawRect(3, 48, 16, 11, WHITE);
        if (digitalRead(4) == LOW) {
          mode = 3;
          while (digitalRead(4) == LOW) {}
        }
        break;
      case 13:
        mode = 8;
        break;

      //-------------------------------------------------------------------------------------------------------------------------OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

      case 14:
        mode = 19;
        break;

      //PUMP TIMER ON SETTING
      case 15:
        display.drawRect(28, 15, 37, 25, WHITE);
        display.setCursor(71, 14);
        display.print("ON ");
        display.setCursor(70, 50);
        display.print("MINUTES");
        if (digitalRead(4) == LOW) {
          delay(20);
          tempSet = pMinOn;
          tempSet = tempSet + 1;
          if (tempSet > 60) {
            tempSet = 1;
          }
          pMinOn = tempSet;
          delay(20);
          previous = 0;
          previousMin = 0;
          pInterval = pMinOff;
          pumpState = false;
          Serial.print("off");
        }
        break;

      //PUMP TIMER OFF SETTING
      case 16:
        display.drawRect(66, 22, 26, 18, WHITE);
        display.setCursor(71, 14);
        display.print("OFF");
        if (hours == 1) {
          display.setCursor(70, 50);
          display.print(" HOURS ");
        }
        if (minutes == 1) {
          display.setCursor(70, 50);
          display.print("MINUTES");
        }
        if (digitalRead(4) == LOW) {
          delay(20);
          if (hours == 1) {
            tempSet = pHourOff;
            tempSet = tempSet + 1;
            if (tempSet > 24) {
              tempSet = 1;
            }
            pHourOff = tempSet;
            delay(20);
            pInterval = pHourOff;
          }
          else if (minutes == 1) {
            tempSet = pMinOff;
            tempSet = tempSet + 1;
            if (tempSet > 60) {
              tempSet = 1;
            }
            pMinOff = tempSet;
            pInterval = pMinOff;
          }
          previous = 0;
          previousMin = 0;
          pumpState = false;
        }
        Serial.print(pMinOff);
        break;

      //SELECT HOURS
      case 17:
        display.setCursor(71, 14);
        display.print("OFF");
        display.setCursor(70, 50);
        display.print(" HOURS ");
        display.drawRect(104, 12, 13, 15, WHITE);
        if (digitalRead(4) == LOW) {
          delay(40);
          hours = 1;
          minutes = 0;
        }
        break;

      //SELECT MINUTES
      case 18:
        display.setCursor(71, 14);
        display.print("OFF");
        display.setCursor(70, 50);
        display.print("MINUTES");
        display.drawRect(104, 26, 13, 15, WHITE);
        if (digitalRead(4) == LOW) {
          delay(40);
          hours = 0;
          minutes = 1;
        }
        break;

      case 19:
        display.drawRect(3, 48, 16, 11, WHITE);
        if (digitalRead(4) == LOW) {
          mode = 4;
          while (digitalRead(4) == LOW) {}
        }
        break;

      case 20:
        mode = 15;
        break;

      //-------------------------------------------------------------------------------------------------------------------------OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

      case 21:
        mode = 0;
        break;

      //HOUR SETTING
      case 22:
        display.setCursor(119, 14);
        display.setTextColor(WHITE, BLACK);
        display.print("H");

        display.setCursor(119, 40);
        display.setTextColor(WHITE, BLACK);
        display.print("M");

        if (flash) {
          display.drawRect(117, 12, 9, 11, WHITE);
        }
        if (digitalRead(4) == LOW) {
          delay(20);
          tempSet = t.hour;
          tempSet = tempSet + 1;
          if (tempSet > 23) {
            tempSet = 0;
          }
          t.hour = tempSet;
          delay(20);
          set_rtc_field(t, hourset);
        }
        break;

      //DAY OF WEEK SETTING
      case 23:
        display.setCursor(119, 14);
        display.setTextColor(WHITE, BLACK);
        display.print("H");

        display.setCursor(119, 40);
        display.setTextColor(WHITE, BLACK);
        display.print("M");

        if (flash) {
          display.drawRect(92, 25, 21, 11, WHITE);
        }
        if (digitalRead(4) == LOW) {
          delay(20);
          tempSet = t.wday;
          tempSet = tempSet + 1;
          if (tempSet > 7) {
            tempSet = 1;
          }
          t.wday = tempSet;
          delay(20);
          set_rtc_field(t, wdayset);
        }
        break;

      //MINUTE SETTING
      case 24:
        display.setCursor(119, 14);
        display.setTextColor(WHITE, BLACK);
        display.print("H");

        display.setCursor(119, 40);
        display.setTextColor(WHITE, BLACK);
        display.print("M");

        if (flash) {
          display.drawRect(117, 38, 9, 11, WHITE);
        }
        if (digitalRead(4) == LOW) {
          tempSet = t.min;
          tempSet = tempSet + 1;
          if (tempSet > 59) {
            tempSet = 0;
          }
          t.min = tempSet;
          set_rtc_field(t, minset);
        }
        break;

      case 25:
        mode = 0;
        break;
    }
    //-----------------------------------------------------END MENU SELECTION STRUCTURE
    prev = now;
    display.display();

    if (virtualPosition != lastCount)
    {
      if (lastCount < virtualPosition)
      {
        mode = mode + 1;
      }
      if (lastCount > virtualPosition)
      {
        mode = mode - 1;
        if (mode < 0) {
          mode = 0;
        }
      }
      lastCount = virtualPosition;
    }

    if (digitalRead(4) == LOW) {
      delay(93);
    }
  }
}

//------------------------------------------------------------------------- END LOOP

byte pumpHours() {
  struct ts t;
  DS3231_get(&t);
  uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };
  byte current = t.hour;
  byte currentMin = t.min;
  if ((pSet) == 3) {
    if (t.sec == 0) {
      Serial.print("new second");
      if (pumpState == 1) {
        if ((currentMin - previousMin) >= pInterval) {
          Serial.print("met minute time criteria");
          pInterval = pHourOff;
          poweredOn1 = 0;
          pumpState = 0;
          Serial.print("off");
        }
        previous = current;
      }
      if (pumpState == 0) {
        if ((current - previous) >= pInterval) {
          Serial.print("met hour time criteria");
          pInterval = pMinOn;
          poweredOn1 = 1;
          pumpState = 1;
          Serial.print("on");
        }
        previousMin = currentMin;
      }
    }
  }
}

byte pumpMinutes() {
  struct ts t;
  DS3231_get(&t);
  uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };
  byte currentMin = t.min;
  if ((pSet) == 3) {
    if (t.sec == 0) { // Only process if we have ticked over to new minute
      Serial.print("new second");
      if ((currentMin - previousMin) >= pInterval) {
        Serial.print("met time criteria");
        if (pumpState == 1) {
          pInterval = pMinOff;
          poweredOn1 = 0;
          pumpState = 0;
          Serial.print("off");
        }
        else if (pumpState == 0) {
          pInterval = pMinOn;
          poweredOn1 = 1;
          pumpState = 1;
          Serial.print("on");
        }
        previousMin = currentMin;
      }
    }
  }
}


void printDay(int day)
{
  switch (day)
  {
    case 1: display.print("Mon "); break;
    case 2: display.print("Tue "); break;
    case 3: display.print("Wed "); break;
    case 4: display.print("Thu "); break;
    case 5: display.print("Fri "); break;
    case 6: display.print("Sat "); break;
    case 7: display.print("Sun "); break;
    default: display.print("ERR "); break;
  }
}

void set_rtc_field(struct ts t,  uint8_t index)
{
  uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(index);
  TimeDate[index] = dectobcd(TimeDate[index]);
  Wire.write(TimeDate[index]);
  Wire.endTransmission();
}

