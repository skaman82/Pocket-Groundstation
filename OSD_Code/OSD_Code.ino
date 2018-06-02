
#include <SPI.h>
#include <max7456.h>

#define redLed 3
#define greenLed 4

int osdpage = 1;
float alarmvalue;
float cellvoltage;


Max7456 osd;
byte logo[]={0xC8,0xC9};

void setup()
{
  SPI.begin();
  Serial.begin(9600);


  osd.init(6);
  osd.setDisplayOffsets(32,18);
  osd.setBlinkParams(_8fields, _BT_BT);
 
  osd.activateOSD();
  
  // mark max screen size to aust offset
  // osd.printMax7456Char(0x01,0,0);
  // osd.printMax7456Char(0x01,0,15);
  // osd.printMax7456Char(0x01,29,0);
  // osd.printMax7456Char(0x01,29,15);
  
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  
  //base time = 160ms,time on = time off.
}



void loop()
 
{
 
  if (osdpage == 1) // Layout1
  { 
  osd.clearScreen();

  // RSSI printout
  osd.printMax7456Char(0x94,24,8);
  osd.print("100",25,8);
 
  // voltage printout
  osd.printMax7456Char(0x90,24,6);
  osd.print("12.4",25,6);

  // DVR printout
  osd.printMax7456Char(0x99,24,7);
  osd.print("REC",25,7);
  
  }
  else {

  osd.printMax7456Chars(logo,2,12,2);
  osd.print("POCKET-GROUNDSTATION",5,9);
  osd.print("WAITING...",10,11, true);
  
    }
 
  delay(100);
}
