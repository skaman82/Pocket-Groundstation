
/*
Notes:
LASEREINHORNBACKFISCH
 */

#define R1 400  // Resistor 1
#define R2 100  // Resistor 2
#define alarmADDR 1 // EEPROM Adress
#define layoutADDR 2 // OSD Layout Adress
#define dvrADDR 3 // DVR Settings Adress

#define Voltagedetect 3.5 // Min. Voltage for Detection

#define note 262  // C4

#define BUTTON1_PIN              2  // Button 1
#define BUTTON2_PIN              3  // Button 2
#define BUTTON3_PIN              4  // Button 3

#define beeppin 9 // Beeper Pin

#define DELAY                    20  // Delay per loop in ms
 
#include "U8glib.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

SoftwareSerial OSDsoft(10, 11); // RX, TX

float voltage;
int lipo;
float alarmvalue = 3.40;
byte alarmvalueEEP;
byte layoutEEP;
byte dvrEEP;
int32_t osddata;
byte blinkosd = 0;
boolean DVRstatus = 0;
boolean RSSIavail;
byte RSSI;
byte VoltageByte;
byte volti = 0; // Counter vor Voltage measure
byte menusel = 0;
byte pressedbut = 0;


const unsigned char splash_bitmap[] PROGMEM = {
  // size is 128 x 64
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##############################....##############################################################################################
  0xFF,0xFF,0xFF,0xFC,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##############################.........#########################################################################################
  0xFF,0xFF,0xFF,0xFC,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##############################..............####################################################################################
  0xFF,0xFF,0xFF,0xFC,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##############################.................#################################################################################
  0xFF,0xFF,0xFF,0xF8,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #############################..................#################################################################################
  0xFF,0xFF,0xFF,0xF8,0xC0,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #############################...##.............#################################################################################
  0xFF,0xFF,0xFF,0xF8,0xCE,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #############################...##..###.......##################################################################################
  0xFF,0xFF,0xFF,0xF8,0x0E,0x63,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #############################.......###..##...##################################################################################
  0xFF,0xFF,0xFF,0xF8,0x00,0x63,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #############################............##...##################################################################################
  0xFF,0xFF,0xFF,0xF0,0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ############################..................##################################################################################
  0xFF,0xFF,0xFF,0xF0,0x00,0x03,0xFF,0xC1,0xE1,0xF0,0xE4,0xC1,0x01,0xFF,0xFF,0xFF, // ############################..................############.....####....#####....###..#..##.....#.......#########################
  0xFF,0xFF,0xFF,0xF0,0xFC,0x07,0xFF,0xC0,0xC0,0xE0,0x64,0xC1,0x01,0xFF,0xFF,0xFF, // ############################....######.......#############......##......###......##..#..##.....#.......#########################
  0xFF,0xFF,0xFF,0xF0,0xFF,0x87,0xFF,0xDC,0x9C,0x4F,0x61,0xCF,0xEF,0xFF,0xFF,0xFF, // ############################....#########....#############.###..#..###...#..####.##....###..#######.############################
  0xFF,0xFF,0xFF,0xF0,0xFF,0x87,0xFF,0xDC,0x9E,0x4F,0xE3,0xCF,0xEF,0xFF,0xFF,0xFF, // ############################....#########....#############.###..#..####..#..#######...####..#######.############################
  0xFF,0xFF,0xFF,0xE1,0xFF,0x87,0xFF,0xC1,0xBE,0x4F,0xE3,0xC1,0xEF,0xFF,0xFF,0xFF, // ###########################....##########....#############.....##.#####..#..#######...####.....####.############################
  0xFF,0xFF,0xFF,0x81,0xFF,0x87,0xFF,0xC3,0x9E,0x4F,0xE1,0xCF,0xEF,0xFF,0xFF,0xFF, // #########################......##########....#############....###..####..#..#######....###..#######.############################
  0xFF,0xFF,0xFC,0x01,0xFF,0x01,0xFF,0xDF,0x8C,0xE6,0x60,0xCF,0xEF,0xFF,0xFF,0xFF, // ######################.........#########.......###########.######...##..###..##..##.....##..#######.############################
  0xFF,0xFF,0xF0,0x01,0xFF,0x00,0x7F,0xCF,0xC0,0xE0,0x64,0x41,0xEF,0xFF,0xFF,0xFF, // ####################...........#########.........#########..######......###......##..#...#.....####.############################
  0xFF,0xFF,0xF0,0x01,0xFF,0x00,0x7F,0xFF,0xF3,0xF9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ####################...........#########.........###################..#######..#################################################
  0xFF,0xFF,0xF0,0x01,0xFF,0x00,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ####################...........#########.........###############################################################################
  0xFF,0xFF,0xF3,0x80,0x00,0x0E,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ####################..###...................###..###############################################################################
  0xFF,0xFF,0xF3,0xFF,0x07,0xFE,0x7F,0xE0,0xE0,0xF0,0xF3,0x33,0x30,0x7F,0xFF,0xFF, // ####################..##########.....##########..##########.....###.....####....####..##..##..##..##.....#######################
  0xFF,0xFF,0xF2,0x7F,0xFF,0xFA,0x7F,0xC0,0xE0,0x60,0x73,0x33,0x30,0x3F,0xFF,0xFF, // ####################..#..####################.#..#########......###......##......###..##..##..##..##......######################
  0xFF,0xFF,0xF3,0xFF,0xFF,0xFE,0x7F,0xCF,0xE6,0x4F,0x33,0x31,0x33,0x9F,0xFF,0xFF, // ####################..#########################..#########..#######..##..#..####..##..##..##...#..##..###..#####################
  0xFF,0xFF,0xF3,0xFF,0xFF,0xFE,0x7F,0x9F,0xE6,0x4F,0x33,0x30,0x33,0x9F,0xFF,0xFF, // ####################..#########################..########..########..##..#..####..##..##..##......##..###..#####################
  0xFF,0xFF,0xF3,0xFF,0xFF,0xFE,0x7F,0x98,0x60,0xCF,0x33,0x30,0x33,0x9F,0xFF,0xFF, // ####################..#########################..########..##....##.....##..####..##..##..##......##..###..#####################
  0xFF,0xFF,0xFB,0x7F,0xFF,0xF2,0x7F,0x9E,0x61,0xCF,0x33,0x32,0x33,0x9F,0xFF,0xFF, // #####################.##.###################..#..########..####..##....###..####..##..##..##..#...##..###..#####################
  0xFF,0xFF,0xFB,0x7F,0xFF,0xF2,0x7F,0xC4,0x64,0xE2,0x32,0x33,0x33,0x3F,0xFF,0xFF, // #####################.##.###################..#..#########...#...##..#..###...#...##..#...##..##..##..##..######################
  0xFF,0xFF,0xFB,0xFF,0xFF,0xFE,0x7F,0xE0,0xE4,0xF0,0x78,0x73,0x30,0x3F,0xFF,0xFF, // #####################.#########################..##########.....###..#..####.....####....###..##..##......######################
  0xFF,0xFF,0xFB,0xFF,0xFF,0xFE,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #####################.#########################..###############################################################################
  0xFF,0xFF,0xF9,0x7F,0xFF,0xF6,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #####################..#.###################.##..###############################################################################
  0xFF,0xFF,0xF9,0x7F,0xFF,0xF6,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // #####################..#.###################.##.################################################################################
  0xFF,0xFF,0xF9,0x7F,0xFF,0xF6,0xFF,0xC3,0x03,0x98,0x09,0xC1,0xCC,0xFF,0xFF,0xFF, // #####################..#.###################.##.##########....##......###..##.......#..###.....###..##..########################
  0xFF,0xFF,0xF9,0xFF,0xFF,0xFE,0xFF,0x89,0xCF,0x1E,0x39,0x88,0xC4,0xFF,0xFF,0xFF, // #####################..########################.#########...#..###..####...####...###..##...#...##...#..########################
  0xFF,0xFF,0xF9,0xFF,0xFF,0xFE,0xFF,0x9F,0xCF,0x0F,0x79,0x3C,0xC4,0xFF,0xFF,0xFF, // #####################..########################.#########..#######..####....####.####..#..####..##...#..########################
  0xFF,0xFF,0xF9,0xBF,0xFF,0xE4,0xFF,0xC3,0xCE,0x4F,0x39,0x3E,0x40,0xFF,0xFF,0xFF, // #####################..##.#################..#..##########....####..###..#..####..###..#..#####..#......########################
  0xFF,0xFF,0xF9,0x9F,0xFF,0xCC,0xFF,0xE1,0xCE,0x07,0x39,0x3E,0x48,0xFF,0xFF,0xFF, // #####################..##..###############..##..###########....###..###......###..###..#..#####..#..#...########################
  0xFF,0xFF,0xF8,0x7F,0xFF,0xF8,0xFF,0xF9,0xCE,0x07,0x39,0x1C,0xC8,0xFF,0xFF,0xFF, // #####################....####################...#############..###..###......###..###..#...###..##..#...########################
  0xFF,0xFF,0xFE,0x39,0xFC,0x61,0xFF,0x89,0xCC,0xE7,0x39,0x80,0xCC,0xFF,0xFF,0xFF, // #######################...###..#######...##....##########...#..###..##..###..###..###..##.......##..##..########################
  0xFF,0xFF,0xFF,0x0D,0xFD,0x87,0xFF,0xC3,0xCC,0xF3,0x39,0xC1,0xCC,0xFF,0xFF,0xFF, // ########################....##.#######.##....#############....####..##..####..##..###..###.....###..##..########################
  0xFF,0xFF,0xFF,0xC3,0x8F,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##########################....###...####...#####################################################################################
  0xFF,0xFF,0xFF,0xF0,0xFC,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ############################....######....######################################################################################
  0xFF,0xFF,0xFF,0xFC,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ##############################..........########################################################################################
  0xFF,0xFF,0xFF,0xFF,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################.....###########################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // ################################################################################################################################
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  // ################################################################################################################################

}
;

const unsigned char settings16_bitmap[] PROGMEM = {
  // size is 16 x 16
  0x03,0xC0, // ......####......
  0x03,0xC8, // ......####..#...
  0x3F,0xFC, // ..############..
  0x7F,0xFE, // .##############.
  0x3F,0xFC, // ..############..
  0x3C,0x3C, // ..####....####..
  0xF8,0x1F, // #####......#####
  0xF8,0x1F, // #####......#####
  0xF8,0x1F, // #####......#####
  0xF8,0x1F, // #####......#####
  0x3C,0x3C, // ..####....####..
  0x3F,0xFC, // ..############..
  0x7F,0xFE, // .##############.
  0x3F,0xFC, // ..############..
  0x13,0xC8, // ...#..####..#...
  0x03,0xC0  // ......####......

}
;




const unsigned char DVR16_bitmap[] PROGMEM = 
{
  // size is 16 x 16
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x1F,0x00, // ...#####........
  0x1F,0x80, // ...######.......
  0x00,0x80, // ........#.......
  0x03,0xC0, // ......####......
  0xFF,0xF1, // ############...#
  0xE7,0x77, // ###..###.###.###
  0xC3,0x77, // ##....##.###.###
  0xC3,0xF7, // ##....######.###
  0xC3,0xF7, // ##....######.###
  0xE7,0xF7, // ###..#######.###
  0xFF,0xF1, // ############...#
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00  // ................
}
;



   

const uint8_t DVRstatus8_bitmap[] U8G_PROGMEM = 
{
  // size is 8  x 8
  0x7E, // .######.
  0x81, // #......#
  0x99, // #..##..#
  0xBD, // #.####.#
  0xBD, // #.####.#
  0x99, // #..##..#
  0x81, // #......#
  0x7E  // .######.
 }
;


void pause()
{
  beep_criticalt(20);
  while (1);
}

void OSDsend()
{
 osddata += (layoutEEP << 19);
 osddata += (blinkosd << 17);
 osddata += (DVRstatus << 16);
 osddata += (RSSIavail << 15);
 osddata += (RSSI << 8);
 osddata += VoltageByte;
  OSDsoft.write(osddata);

}


void setup()  
{
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  
  alarmvalueEEP = EEPROM.read(alarmADDR);
  layoutEEP = EEPROM.read(layoutADDR);
  dvrEEP = EEPROM.read(dvrADDR);
  
  if (alarmvalueEEP != 0)
  {
    alarmvalue = (alarmvalueEEP / 10.0);
  }
  else {
        alarmvalue = 3.40;

    }

  Serial.begin(9600);
  OSDsoft.begin(9600);

  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }

  clearOLED();
  showlogo();
  delay(100);



 


  voltagetest();
  if (voltage > (Voltagedetect * 5.0))
  {
    lipo = 5;
    beep_x(lipo);
  }
  else if (voltage > (Voltagedetect * 4.0))
  {
    lipo = 4;
    beep_x(lipo);
  }
  else if (voltage > (Voltagedetect * 3.0))
  {
    lipo = 3;
    beep_x(lipo);
  }
  else if (voltage > (Voltagedetect * 2.0))
  {
    lipo = 2;
    beep_x(lipo);
  }
  else
  {
    pause();
  }
}



void clearOLED(){
    u8g.firstPage();  
    do {
    } while( u8g.nextPage() );
}

void showlogo(){
 u8g.firstPage();
  do {
// splashscreen goes here
    u8g.drawBitmapP(0, 0, 16, 64, splash_bitmap);
    }
      while (u8g.nextPage());
      delay(1500);
      clearOLED();


}


void voltagetest() 
{

  int sensorValue = analogRead(A0); // read the input on analog pin 0:
  voltage = sensorValue * (5.0 / 1023.0) * ((R1 + R2) / R2); // Convert the analog reading (which goes from 0 - 1023) to a voltage, considering the voltage divider:
  // Serial.println(voltage);   // print out the value you read:
  voltage = round(voltage * 10) / 10.0; //round the result

  float cellvoltage = voltage / lipo;

  u8g.drawFrame(96, 16 - 1, 29, 20);
  u8g.drawBox(96, 16, 29, 6);
  u8g.setPrintPos(101, 31);
  u8g.print(alarmvalue);
  u8g.setColorIndex(0);
  u8g.setPrintPos(101, 21);
  u8g.setFont(u8g_font_micro);
  u8g.print("ALARM");
  u8g.setColorIndex(1);
  
  /*
  delay(10);
   Serial.println(cellvoltage, 1);
   */

  if (cellvoltage > 4.0) // case if voltage is above 4.0v
  { 
    u8g.drawBox(2, 5, 2, 2);
    u8g.drawBox(5, 5, 2, 2);
    u8g.drawBox(8, 5, 2, 2);
    u8g.drawFrame(0, 5 - 2, 12, 6);
    u8g.drawBox(12, 5, 1, 2);
  } 
  else if (cellvoltage < 3.9 && voltage > 3.4) // case if voltage is below 3.4
  {
    u8g.drawBox(2, 5, 2, 2);
    u8g.drawBox(5, 5, 2, 2);
    u8g.drawFrame(0, 5 - 2, 12, 6);
    u8g.drawBox(12, 5, 1, 2);
  } 
  else if (cellvoltage < 3.4) // case if voltage is below 3.4
  { 
    
    u8g.drawBox(2, 5, 2, 2);
    u8g.drawFrame(0, 5 - 2, 12, 6);
    u8g.drawBox(12, 5, 1, 2);
  } 
  else 
  {
    u8g.drawFrame(0, 5 - 2, 12, 6);
    u8g.drawBox(12, 5, 1, 2);
  }

  if (cellvoltage < (alarmvalue)) // case if voltage is under the set alarm value
  { 
    beep_criticalt(1);
  } 

}

byte buttoncheck()
{
  byte buttonz = 0;
  if (digitalRead(BUTTON1_PIN) != 1)
  {
    while(digitalRead(BUTTON1_PIN) != 1)
    {
    }
    buttonz = 1;
  }
  else if (digitalRead(BUTTON2_PIN) != 1)
  {
    while(digitalRead(BUTTON2_PIN) != 1)
    {
    }
    buttonz = 2;
  }
  else if (digitalRead(BUTTON3_PIN) != 1)
  {
    while(digitalRead(BUTTON3_PIN) != 1)
    {
    }
    buttonz = 3;
  }
  pressedbut = buttonz;
  return buttonz;
}




void loop() 
{
  // handle buttons

  int buttonState1 = !digitalRead(BUTTON1_PIN); // pin low -> pressed
  if (buttonState1 == 1) 
  {
    //beep(1);
    menu();
    Serial.print("Button1 pressed");
  }

  int buttonState2 = !digitalRead(BUTTON2_PIN); // pin low -> pressed
  if (buttonState2 == 1) 
  {
    //beep(1);
    Serial.print("Button2 pressed");
    if (alarmvalue > 2.70) 
    {
      alarmvalue -= 0.10;
      alarmvalueEEP = alarmvalue * 10.0;
      EEPROM.write(alarmADDR, alarmvalueEEP);
    }
  }

  int buttonState3 = !digitalRead(BUTTON3_PIN); // pin low -> pressed
  if (buttonState3 == 1) 
  {
    //beep(1);
    Serial.print("Button3 pressed");
    if (alarmvalue < 3.50) 
    {
      alarmvalue += 0.10;
      alarmvalueEEP = alarmvalue * 10.0;
      EEPROM.write(alarmADDR, alarmvalueEEP);
    }
  }
  
  volti++;
  //clearOLED();
  if(volti == 20)
  {
    voltagetest();
    OSDsend();
    volti = 0;
  }
  
  u8g.firstPage();
  do {
    // graphic commands to redraw the complete screen should be placed here
    u8g.setFont(u8g_font_5x7);
    u8g.setPrintPos(34, 9);
    u8g.print("BATTERY");
    u8g.setFont(u8g_font_5x7);
    u8g.setPrintPos(18, 9);
    u8g.print(lipo);
    u8g.setFont(u8g_font_5x7);
    u8g.setPrintPos(24, 9);
    u8g.print("S");
    u8g.setFont(u8g_font_profont22);
    u8g.setPrintPos(0, 32);
    u8g.print(voltage, 1);
    if (voltage > 10.0) {
    u8g.setPrintPos(52, 32);
    u8g.print("v");
    }
    else if (voltage < 10.0) {
    u8g.setPrintPos(40, 32);
    u8g.print("v");
      }

    u8g.drawBitmapP(96, 2, 1, 8, DVRstatus8_bitmap);

    u8g.drawFrame(1, 46 - 2, 124, 18);
    u8g.setFont(u8g_font_5x7);
    u8g.setPrintPos(30, 55);
    u8g.print("Press for Menu");

    u8g.setPrintPos(111, 9);
    u8g.print("OSD");

  }
  while (u8g.nextPage());
  
  
  
  delay(DELAY);

}

void menu() 
{
  byte exit = 0;
  while(exit == 0)
  {
    clearOLED();
    u8g.firstPage();
    do
    {
      if(menusel == 0)
      {
        u8g.drawBox(1, 6 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 16);
        u8g.setColorIndex(0);
        u8g.print("DVR MODE");
        u8g.setColorIndex(1);
        
        u8g.drawFrame(1, 26 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 36);
        u8g.print("SETTINGS");
        
        u8g.drawFrame(1, 46 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 56);
        u8g.print("EXIT");
      }
      else if(menusel == 1)
      {
        u8g.drawFrame(1, 6 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 16);
        u8g.print("DVR MODE");
        
        u8g.drawBox(1, 26 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 36);
        u8g.setColorIndex(0);
        u8g.print("SETTINGS");
        u8g.setColorIndex(1);
        
        u8g.drawFrame(1, 46 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 56);
        u8g.print("EXIT");
      }
      else
      {
        u8g.drawFrame(1, 6 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 16);
        u8g.print("DVR MODE");
        
        u8g.drawFrame(1, 26 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 36);
        u8g.print("SETTINGS");
        
        u8g.drawBox(1, 46 - 2, 124, 18);
        u8g.setFont(u8g_font_5x7);
        u8g.setPrintPos(10, 56);
        u8g.setColorIndex(0);
        u8g.print("EXIT");
        u8g.setColorIndex(1);
      }
    }
    while (u8g.nextPage());
    
    pressedbut = buttoncheck();
    while(pressedbut == 0)
    {
      buttoncheck();
    }
    if(pressedbut == 1)
    {
      // Press selected Menu Point
      if(menusel == 2)
      {
        exit = 1;
      }
    }
    else if(pressedbut == 2)
    {
      if(menusel > 0)
      {
        menusel--;
      }
    }
    else if(pressedbut == 3)
    {
      if(menusel < 2)
      {
        menusel++;
      }
    }
  }
  

}
    

// Beep-Stuff

void beep(unsigned char delayms) 
{
  tone(beeppin, note, 10); // 10ms beep (C4 Tone)
}

void beep_long(unsigned char delayms) 
{
  tone(beeppin, note, 2000); // 2000ms beep (C4 Tone)
}

void beep_warning(unsigned char delayms) 
{
  tone(beeppin, note, 200); // 200ms beep (C4 Tone)
  delay(400);
  tone(beeppin, note, 200); // 200ms beep (C4 Tone)
}

void beep_criticalt(unsigned char delayms) 
{
  tone(beeppin, note, 400); // 400ms beep (C4 Tone)
  delay(800);

}

void beep_x(byte b) 
{
  for (int i = 1; i <= b; i++) 
  {
    tone(beeppin, note, 200);
    delay(400);
  }
}









