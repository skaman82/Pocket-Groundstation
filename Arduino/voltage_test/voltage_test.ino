
/*
Notes:
LASEREINHORNBACKFISCH
 */

#define R1 400  // Widerstand 1
#define R2 100  // Widerstand 2

#define note 262  // C4

#define BUTTON1_PIN              2  // Button 1
#define BUTTON2_PIN              3  // Button 2
#define BUTTON3_PIN              4  // Button 3

#define DELAY                    20  // Delay per loop in ms
 
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI




boolean button_was_pressed = false;; // previous state

float voltage;



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


  beep(10);
}

boolean handle_button1()
{
  boolean event;
  int button_now_pressed = !digitalRead(BUTTON1_PIN); // pin low -> pressed

  event = button_now_pressed && !button_was_pressed;
  button_was_pressed = button_now_pressed;
  return event;
}

boolean handle_button2()
{
  boolean event;
  int button_now_pressed = !digitalRead(BUTTON2_PIN); // pin low -> pressed

  event = button_now_pressed && !button_was_pressed;
  button_was_pressed = button_now_pressed;
  return event;
}

boolean handle_button3()
{
  boolean event;
  int button_now_pressed = !digitalRead(BUTTON3_PIN); // pin low -> pressed

  event = button_now_pressed && !button_was_pressed;
  button_was_pressed = button_now_pressed;
  return event;
}


void voltagetest() 
{
  int sensorValue = analogRead(A0);   // read the input on analog pin 0:
  voltage = sensorValue * (5.0 / 1023.0) * ((R1+R2)/R2);   // Convert the analog reading (which goes from 0 - 1023) to a voltage, considering the voltage divider:
 // Serial.println(voltage);   // print out the value you read:


  if (voltage > 4.9) 
  { // case if voltage is above 4.9v
    Serial.print("Battery charging");
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
  u8g.print("4S");
  u8g.setFont(u8g_font_8x13B);
  u8g.setPrintPos(0, 24);
  u8g.print(voltage), (" V");
  delay(10);
}




void loop()  
{
   // handle button
  boolean raising_edge1 = handle_button1();
  boolean raising_edge2 = handle_button2();
  boolean raising_edge3 = handle_button3();

  // do other things
  Serial.print(raising_edge1 ? "1" : ".");
  Serial.print(raising_edge2 ? "2" : ".");
  Serial.print(raising_edge3 ? "3" : ".");

  // add newline sometimes
  static int counter = 0;
  if ((++counter & 0x3f) == 0)
    Serial.println();
    delay(100);
    
  u8g.firstPage();
  do {
    voltagetest();
  }
  while (u8g.nextPage());
  delay(DELAY);
}

void beep(unsigned char delayms) 
{
  tone(9, note, 100);  // 100ms beep (C4 Tone)
}

void beep_long(unsigned char delayms) 
{
  tone(9, note, 2000); // 2000ms beep (C4 Tone)
}

void beep_warning(unsigned char delayms) 
{
  tone(9, note, 200);  // 200ms beep (C4 Tone)
  delay(400);
  tone(9, note, 200);  // 200ms beep (C4 Tone)
}

void beep_criticalt(unsigned char delayms) 
{
  tone(9, note, 400);  // 400ms beep (C4 Tone)
  delay(800);
  tone(9, note, 400);  // 400ms beep (C4 Tone)
  delay(800);
  tone(9, note, 400);  // 400ms beep (C4 Tone)
}











