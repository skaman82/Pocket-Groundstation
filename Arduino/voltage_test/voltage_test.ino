
/*
Notes:
LASEREINHORNBACKFISCH
 */

#define R1 400  // Widerstand 1
#define R2 100  // Widerstand 2

#define Voltagedetect 3.5 // Min. Voltage for Detection

#define note 262  // C4

#define BUTTON1_PIN              2  // Button 1
#define BUTTON2_PIN              3  // Button 2
#define BUTTON3_PIN              4  // Button 3

#define beeppin 9 // Beeper Pin

#define DELAY                    20  // Delay per loop in ms
 
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI




float voltage;

int lipo;



const uint8_t rook_bitmap[] PROGMEM = 
{
  // size is 8 x 7
  0x0,
  0x00,
  0x1c,
  0x7f,
  0xfc,
  0x7f,
  0x1c,
  0x00

};

void pause()
{
  beep_criticalt(20);
  while (1);
}

void setup()  
{

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  
  Serial.begin(9600);

  

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




void voltagetest() 
{
  int sensorValue = analogRead(A0);   // read the input on analog pin 0:
  voltage = sensorValue * (5.0 / 1023.0) * ((R1+R2)/R2);   // Convert the analog reading (which goes from 0 - 1023) to a voltage, considering the voltage divider:
 // Serial.println(voltage);   // print out the value you read:


  if (voltage > 4.9) 
  { // case if voltage is above 4.9v
    u8g.drawBox(114, 5, 2, 2);
    u8g.drawBox(117, 5, 2, 2);
    u8g.drawBox(120, 5, 2, 2);
    u8g.drawFrame(112, 5 - 2, 12, 6);
    u8g.drawBox(124, 5, 1, 2);

    u8g.setFont(u8g_font_5x7);
    u8g.setPrintPos(43, 9);
    u8g.drawBitmapP( 78, 2, 1, 8, rook_bitmap);

  }

  else if (voltage < 3.9 && voltage > 3.4) 
  { // case if voltage is below 3.4
    Serial.print("Battery half");
    u8g.drawBox(114, 5, 2, 2);
    u8g.drawBox(117, 5, 2, 2);
    u8g.drawFrame(112, 5 - 2, 12, 6);
    u8g.drawBox(124, 5, 1, 2);
  }

  else if (voltage < 3.4) 
  { // case if voltage is below 3.4
    beep_criticalt(225);
    Serial.print("Battery critical");
    u8g.drawFrame(112, 5 - 2, 12, 6);
    u8g.drawBox(124, 5, 1, 2);

  }

  else 
  {
    u8g.drawBox(114, 5, 2, 2);
    u8g.drawBox(117, 5, 2, 2);
    u8g.drawBox(120, 5, 2, 2);
    u8g.drawFrame(112, 5 - 2, 12, 6);
    u8g.drawBox(124, 5, 1, 2);

  }

  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(0, 9);
  u8g.print("Battery Status");
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(99, 9);
  u8g.print(lipo);
   u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(105, 9);
  u8g.print("S");
  u8g.setFont(u8g_font_8x13B);
  u8g.setPrintPos(0, 24);
  u8g.print(voltage), (" V");
  delay(10);
}




void loop()  {
 // handle buttons
    
    int buttonState1 = !digitalRead(BUTTON1_PIN); // pin low -> pressed
    if (buttonState1 == 1)
    {
     beep(1);
     Serial.print("Button1 pressed");
    }
    
    
    int buttonState2 = !digitalRead(BUTTON2_PIN); // pin low -> pressed
    if (buttonState2 == 1)
    {
     beep(1);
     Serial.print("Button2 pressed");
    }

    
    int buttonState3 = !digitalRead(BUTTON3_PIN); // pin low -> pressed
    if (buttonState3 == 1)
    {
     beep(1);
     Serial.print("Button3 pressed");
    }
    
 

  // add newline sometimes

  /*
   *  Find ich komisch
   *
  static int counter = 0;
  if ((++counter & 0x3f) == 0)
    Serial.println();
    delay(100);

  */
    
  u8g.firstPage();
  do {
    voltagetest();
  }
  while (u8g.nextPage());
  delay(DELAY);
}

// Beep-Stuff

void beep(unsigned char delayms) 
{
  tone(beeppin, note, 10);  // 100ms beep (C4 Tone)
}

void beep_long(unsigned char delayms) 
{
  tone(beeppin, note, 2000); // 2000ms beep (C4 Tone)
}

void beep_warning(unsigned char delayms) 
{
  tone(beeppin, note, 200);  // 200ms beep (C4 Tone)
  delay(400);
  tone(beeppin, note, 200);  // 200ms beep (C4 Tone)
}

void beep_criticalt(unsigned char delayms) 
{
  tone(beeppin, note, 400);  // 400ms beep (C4 Tone)
  delay(800);
  tone(beeppin, note, 400);  // 400ms beep (C4 Tone)
  delay(800);
  tone(beeppin, note, 400);  // 400ms beep (C4 Tone)
}

void beep_x(byte b)
{
  for (int i = 1; i <= b; i++)
  {
    tone(beeppin, note, 200);
    delay(400);
  }
}









