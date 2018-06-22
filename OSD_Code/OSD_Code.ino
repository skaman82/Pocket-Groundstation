
#include <SPI.h>
#include <max7456.h>

#define redLed 3
#define greenLed 4

float alarmvalue;
float cellvoltage;
int32_t osddata = 0;
int32_t layoutEEP = 0;
int32_t oldlayout;
int32_t blinkosd = 0;
int32_t DVRstatus = 0;
int32_t RSSIavail = 0;
int32_t oldRSSI;
int32_t oldDVR;
int32_t RSSI = 0;
int32_t VoltageByte = 0;
float Voltage;
int32_t battery_health = 0;

int runXTimes = 1;

Max7456 osd;
byte logo1[]={0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC };
byte logo2[]={0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC };
byte logo3[]={0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC };
byte logo4[]={0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC };


void setup()
{
  SPI.begin();
  Serial.begin(9600);


  osd.init(6);
  osd.setDisplayOffsets(32,18);
  osd.setBlinkParams(_8fields, _BT_BT);
 
  osd.activateOSD();
  
  // mark max screen size to adjust offset
  // osd.printMax7456Char(0x01,0,0);
  // osd.printMax7456Char(0x01,0,15);
  // osd.printMax7456Char(0x01,29,0);
  // osd.printMax7456Char(0x01,29,15);
  
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  
  //base time = 160ms,time on = time off.

  splash();
}

void OSDreceive()
{
  delay(2);
  byte b1 = Serial.read();
  byte b2 = Serial.read();
  byte b3 = Serial.read();
  byte b4 = Serial.read();
  
  osddata = ((int32_t)b1 << 24);
  osddata += ((int32_t)b2 << 16);
  osddata += ((int32_t)b3 << 8);
  osddata += b4;
  
  battery_health = (0x380000 & osddata) >> 19;
  layoutEEP = (0x60000 & osddata) >> 17;
  DVRstatus = (0x10000 & osddata) >> 16;
  RSSIavail = (0x8000 & osddata) >> 15;
  RSSI = (0x7F00 & osddata) >> 8;
  VoltageByte = 0xFF & osddata;
  
  /*
  osddata = (battery_health << 21);
  layoutEEP = osddata >> 19;
  blinkosd = osddata >> 17;
  DVRstatus = osddata >> 16;
  RSSIavail = osddata >> 15;
  RSSI = osddata >> 8);
  VoltageByte = osddata);
  */
  
}
void splash()
{
  osd.printMax7456Chars(logo1,12,9,5);
  osd.printMax7456Chars(logo2,12,9,6);
  osd.printMax7456Chars(logo3,12,9,7);
  osd.printMax7456Chars(logo4,12,9,8);
  delay(2000);
  osd.clearScreen();
  }


 void clearscreen()
  {
  if (runXTimes)
  {
    osd.clearScreen();
     runXTimes--;
     
    }
  }

void checkChanges()
  {
  if (RSSIavail != oldRSSI)
    {
    osd.clearScreen();
    oldRSSI = RSSIavail;
    }
   else if (layoutEEP != oldlayout)
    {
    osd.clearScreen();
    oldlayout = layoutEEP;
    }   
    else if (DVRstatus != oldDVR)
    {
    osd.clearScreen();
    oldDVR = DVRstatus;
    }   
    else { }
  }

  
void loop()
{
  if (Serial.available() > 0)
  {
    OSDreceive();
    checkChanges();
  }
 double Osvoltage = (VoltageByte / 10.0);
 double OslayoutEEP = layoutEEP;
 double OsRSSI = RSSI;
 double OsDVRstatus = DVRstatus;
 double Osblinkosd = blinkosd;
 double OsRSSIavail = RSSIavail;
 double OsHealth = battery_health;
 
  
  
  
  if (layoutEEP == 1) // Layout1
  { 
  clearscreen();
  oldlayout = layoutEEP;

  if (RSSIavail == 1) 
    {
    // RSSI printout
    osd.printMax7456Char(0x94,24,6);
    osd.print(OsRSSI,25,6, 2, 0);
    osd.printMax7456Char(0x7F,27,6); //black 
    osd.printMax7456Char(0x7F,28,6); //black 
    osd.printMax7456Char(0x7F,29,6); //black 
    oldRSSI = RSSIavail;
    }
    else {   }

    // voltage printout
    
    
    if (battery_health == 4) 
    {
      osd.printMax7456Char(0x90,24,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
    else if (battery_health == 3) 
    {
      osd.printMax7456Char(0x91,24,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
     else if (battery_health == 2) 
    {
      osd.printMax7456Char(0x92,24,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
     else if (battery_health == 1) 
    {
      osd.printMax7456Char(0x93,24,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
    else
    {
      osd.printMax7456Char(0x93,24,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }    

      
    if (Osvoltage > 10) {
    osd.print(Osvoltage, 25, 7, 2, 1); 
    
    }
    else if (Osvoltage < 10) {
    osd.print(Osvoltage, 25, 7, 1, 1);
    osd.printMax7456Char(0x7F,28,7); //black 
    }
    
    
  // DVR printout
  if (DVRstatus == 1) 
  {
    osd.printMax7456Char(0x99,24,8);
    osd.print("REC",25,8);
    osd.printMax7456Char(0x7F,28,8); //black 
    osd.printMax7456Char(0x7F,29,8); //black 
  }
  else {}


  
  

  osd.print("VOLTAGE",1,3);
  osd.print(Osvoltage, 9, 3, 2, 1); // test for voltage
  
  osd.print("OSD LAY",1,4);
  osd.print(OslayoutEEP, 9, 4, 1, 0); // test for OSD layout

  osd.print("RSSI",1,5);
  osd.print(OsRSSI, 9, 5, 2, 0); // test for RSSI strenght

  osd.print("DVR",1,6);
  osd.print(OsDVRstatus, 9, 6, 1, 0); // test for DVR status

  osd.print("BLINK",1,7);
  osd.print(Osblinkosd, 9, 7, 1, 0); // test for alarm status

  osd.print("RS-A",1,8);
  osd.print(OsRSSIavail, 9, 8, 1, 0); // test if RSSI is available

  osd.print("HEALTH",1,9);
  osd.print(OsHealth, 9, 9, 1, 0); // test if battery-health is available
  
  
  }

  
  else if (OslayoutEEP == 2) {
  //clearscreen();
  //osd.print("POCKET-GROUNDSTATION",5,9);
  //osd.print("WAITING...",10,11, true);

  clearscreen();
  oldlayout = layoutEEP;

  if (RSSIavail == 1) 
    {
    // RSSI printout
    osd.printMax7456Char(0x94,28,6);
    osd.printMax7456Char(0x7F,29,6); //black 
    oldRSSI = RSSIavail;
    }
    else {   }

    // voltage printout
    
    if (battery_health == 4) 
    {
      osd.printMax7456Char(0x90,28,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
    else if (battery_health == 3) 
    {
      osd.printMax7456Char(0x91,28,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
     else if (battery_health == 2) 
    {
      osd.printMax7456Char(0x92,28,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
     else if (battery_health == 1) 
    {
      osd.printMax7456Char(0x93,28,7);
      osd.printMax7456Char(0x7F,29,7); //black 
    }
    else
    {
      osd.printMax7456Char(0x93,28,7, true);
      osd.printMax7456Char(0x7F,29,7, true); //black 
    }    
    
    
  // DVR printout
  if (DVRstatus == 1) 
  {
    osd.printMax7456Char(0x99,28,8);
    osd.printMax7456Char(0x7F,29,8); //black 
  }
  else {}

 
    }

 else  
  {
  }
 
  delay(100);
}
