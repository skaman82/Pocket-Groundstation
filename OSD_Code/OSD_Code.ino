#include <SPI.h>
#include <max7456.h>

#define redLed 3
#define greenLed 4

Max7456 osd;
unsigned long counter = 0;
byte tab[]={0xC8,0xC9};

void setup()
{
  SPI.begin();
  
  osd.init(10);
  osd.setDisplayOffsets(35,25);
  osd.setBlinkParams(_8fields, _BT_BT);
 
  osd.activateOSD();
  osd.printMax7456Char(0x01,0,0);
  osd.print("LASEREINHORNBACKFISCH",1,3);
  osd.print("CURRENT ARDUINO TIME :",1,4);

  osd.printMax7456Char(0xD1,9,6,true);
  osd.print("00'00\"",10,6);  
  osd.printMax7456Chars(tab,2,12,7);
 
  osd.printMax7456Char(0x90,10,1);
  osd.print("12.4",11,1);


  
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  
  //base time = 160ms,time on = time off.
}


void loop()
{
  if(counter%2 == 0)
  {
    digitalWrite(redLed,LOW);
    digitalWrite(greenLed,HIGH);
  }
  else
  {
    digitalWrite(redLed,HIGH);
    digitalWrite(greenLed,LOW);
  }

  counter = millis()/1000;
  
  osd.print(int(counter/60),10,6,2,0,false,true);
  osd.print(int(counter%60),13,6,2,0,false,true);
  
  delay(100);
}
