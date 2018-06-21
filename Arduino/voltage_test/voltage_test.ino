
/*
Notes:
LASEREINHORNBACKFISCH
 */

#include "definesGS.h"
#include "U8glib.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>

// change this depending on display type
// U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

SoftwareSerial OSDsoft(10, 11); // RX, TX

float voltage;
int32_t lipo;
float alarmvalue = 3.40;
int32_t alarmvalueEEP;
int32_t DVRstatus = 0;
int32_t layoutEEP = 0;
int32_t rssiEEP = 0;
int dvrEEP = 0;
int32_t osddata;
int32_t RSSIavail;
int32_t RSSI = 69;
int32_t VoltageByte;
int refreshi = 0;
int volti = 0; // Counter for Voltage measure
int menusel = 0;
int pressedbut = 0;
float cellvoltage;
int osdON;
int32_t battery_health = 0;
int dvr_sensor;
unsigned long timeDVRblink = 0;
unsigned long LEDMillis = 0;
const long interval = 2000;
int LEDState = LOW;

#include "bitmaps.h"

void pause()
{
    while (1);
}

void OSDsend()
{
    osddata = (battery_health << 19);
    osddata += (layoutEEP << 17);
    osddata += (DVRstatus << 16);
    osddata += (RSSIavail << 15);
    osddata += (RSSI << 8);
    osddata += VoltageByte;
    //int32_t dat = 0x80C0E0F0;

    byte b1 = ((osddata >> 24) & 0xFF);
    byte b2 = ((osddata >> 16) & 0xFF);
    byte b3 = ((osddata >> 8) & 0xFF);
    byte b4 = (osddata & 0xFF);

    OSDsoft.write(b1);
    //delay(2)
    OSDsoft.write(b2);
    //delay(2)
    OSDsoft.write(b3);
    //delay(2)
    OSDsoft.write(b4);
    //delay(2)
    //Serial.println(osddata);

    //OSDsoft.print(osddata);
    
    RSSIavail = rssiEEP;

}


void setup()
{
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);

    pinMode(DVR1_PIN, OUTPUT); //DVR Key1
    pinMode(DVR2_PIN, OUTPUT); //DVR Key2
    pinMode(DVR3_PIN, OUTPUT); //DVR Key3
    pinMode(DVR_SENS, INPUT); //DVR LED Sensor

    pinMode(STATUS_LED, OUTPUT); //StatusLED

    digitalWrite(DVR1_PIN, HIGH);
    digitalWrite(DVR2_PIN, HIGH);
    digitalWrite(DVR3_PIN, HIGH);

    
    alarmvalueEEP = EEPROM.read(alarmADDR);
    layoutEEP = EEPROM.read(layoutADDR);
    dvrEEP = EEPROM.read(dvrADDR);
    rssiEEP = EEPROM.read(rssiADDR);

    
    if (alarmvalueEEP != 0)
    {
        alarmvalue = (alarmvalueEEP / 10.0);
    }
    else
    {
        alarmvalue = 3.40;
    }


    Serial.begin(9600);
    OSDsoft.begin(9600);


    if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
    {
        u8g.setColorIndex(255);     // white
    }
    else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) 
    {
        u8g.setColorIndex(3);         // max intensity
    }
    else if ( u8g.getMode() == U8G_MODE_BW ) 
    {
        u8g.setColorIndex(1);         // pixel on
    }
    else if ( u8g.getMode() == U8G_MODE_HICOLOR ) 
    {
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
        //pause();
        lipo = 1;
        beep_x(lipo);
    }
}


void clearOLED() 
{
    u8g.firstPage();
    do 
    {
    } 
    while( u8g.nextPage() );
}


void showlogo()
{
    u8g.firstPage();
    do 
    {
      // splashscreen goes here
      u8g.drawBitmapP(15, 10, 12, 46, splash_bitmap);
    }
    while (u8g.nextPage());
    delay(1500);
    clearOLED();
}


void voltagetest()
{

    int sensorValue = analogRead(A0); // read the input on analog pin 0:
    voltage = sensorValue * (5.38 / 1023.0) * ((R1 + R2) / R2); // Convert the analog reading (which goes from 0 - 1023) to a voltage, considering the voltage divider:
    // Serial.println(voltage);   // print out the value you read:
    voltage = round(voltage * 10) / 10.0; //round the result
    VoltageByte = voltage*10;

    cellvoltage = voltage / lipo;

    if (cellvoltage < (alarmvalue)) // case if voltage is under the set alarm value
    {
       tone(beeppin, note, 400); // 400ms beep (C4 Tone)
    }


    float cellfull = (max_cellvoltage) - (alarmvalue); //determine 100% of travel scale
    float cellstate = (max_cellvoltage) - (cellvoltage); //determine the actual cell delta value
    float battery_state = 100 - (((cellstate)*100) / (cellfull)); //determine cell charge left in percent

    if (battery_state >= 75 && battery_state < 100) 
    {
        battery_health = 4;
    }
    else if (battery_state >= 50 && battery_state < 75) 
    {
        battery_health = 3;
    }
    else if (battery_state >= 25 && battery_state < 50) 
    {
        battery_health = 2;
    }
    else if (battery_state >= 5 && battery_state < 25) 
    {
        battery_health = 1;
    }
    else if (battery_state >= 0) 
    {
        battery_health = 0;
    }
    else {
        battery_health = 0;
    }
    
}


byte buttoncheck()
{
    int i_butt = 0;
    byte buttonz = 0;
    if (digitalRead(BUTTON1_PIN) != 1)
    {
        while(digitalRead(BUTTON1_PIN) != 1)
        {
            delay(2);
            i_butt++;
        }
        buttonz = 1;
        tone(beeppin, note, 10); // 10ms beep (C4 Tone)
        
        if (i_butt > (longpresstime / 2))
        {
          buttonz += 3;
          tone(beeppin, note, 100); // 100ms beep (C4 Tone)
        }
    }

    else if (digitalRead(BUTTON2_PIN) != 1)
    {
        while(digitalRead(BUTTON2_PIN) != 1)
        {
            delay(2);
            i_butt++;
        }
        buttonz = 2;
        tone(beeppin, note, 10); // 10ms beep (C4 Tone)
        
        if (i_butt > (longpresstime / 2))
        {
          buttonz += 3;
          tone(beeppin, note, 100); // 100ms beep (C4 Tone)
        }

    }
    else if (digitalRead(BUTTON3_PIN) != 1)
    {
        while(digitalRead(BUTTON3_PIN) != 1)
        {
            delay(2);
            i_butt++;
        }
        buttonz = 3;
        tone(beeppin, note, 10); // 10ms beep (C4 Tone)
        
        if (i_butt > (longpresstime / 2))
        {
          buttonz += 3;
          tone(beeppin, note, 100); // 100ms beep (C4 Tone)
        }
    }
    //delay(10);
    pressedbut = buttonz;
    return buttonz;
}


void ledcheck() 
{

 if (battery_health > 0) 
 {
   dvr_sensor = analogRead(DVR_SENS);
    if  (dvr_sensor < 300) 
  {
  digitalWrite(STATUS_LED, HIGH); 
  }
  else 
  {
    digitalWrite(STATUS_LED, LOW); 
    }
}
  else if (battery_health == 0)
  {
   unsigned long currentLEDMillis = millis();
    if ((currentLEDMillis - LEDMillis >= 300))
   {
      LEDMillis = currentLEDMillis;
 
       if (LEDState == LOW) {
          LEDState = HIGH;
        } 
        else {
          LEDState = LOW;
        } 
    digitalWrite(STATUS_LED, LEDState); 
  }
  
}
}


void loop()
{


    if (layoutEEP == 1)
    {
        osdON = true;
    }
    else if (layoutEEP == 2)
    {
        osdON = true;
    }
    else if (layoutEEP == 0)
    {
        osdON = false;
    }
    else {
        osdON = false;
    }


    // Serial.print("health: ");
    // Serial.print(battery_health);

    // Serial.print(" state: ");
    // Serial.print(battery_state);

    Serial.print(" alarmvalueEEP: ");
    Serial.print(alarmvalueEEP);

     Serial.print(" layoutEEP: ");
     Serial.print(layoutEEP);

     // Serial.print(" DVRstatus: ");
     // Serial.print(DVRstatus);

     Serial.print(" VoltageByte: ");
     Serial.print(VoltageByte);

    // Serial.print(" dvrEPP: ");
    // Serial.print(dvrEEP);

    Serial.println();

    ledcheck();

    refreshi++;
    if(refreshi > 5)
    {
        volti++;
        if(volti > 5)
        {
            voltagetest();
            volti = 0;
        }
        u8g.firstPage();

        OSDsend();

        do {
            // graphic commands to redraw the complete screen should be placed here
            u8g.setFont(u8g_font_5x7);
            u8g.setPrintPos(33, 9);
            u8g.print("BATTERY");
            u8g.setFont(u8g_font_5x7);
            u8g.setPrintPos(17, 9);
            u8g.print(lipo);
            u8g.setFont(u8g_font_5x7);
            u8g.setPrintPos(23, 9);
            u8g.print("S");
            u8g.setFont(u8g_font_profont22);
            u8g.setPrintPos(0, 32);
            u8g.print(voltage, 1);



            if (voltage > 10.0) {
                u8g.setPrintPos(52, 32);
                u8g.print("v");
            }
            else if (voltage < 10.0)
            {
                u8g.setPrintPos(40, 32);
                u8g.print("v");
            }


            u8g.drawFrame(1, 46 - 2, 124, 18);
            u8g.setFont(u8g_font_5x7);
            u8g.setPrintPos(12, 56);
            u8g.print("PRESS CENTER FOR MENU");


            dvr_sensor = analogRead(DVR_SENS);
            if (dvr_sensor < 300) //if DVR LED is blinking
            {   
                DVRstatus = 1;  
                timeDVRblink = millis();
            } 
            
            else
            {
                if(millis() - timeDVRblink > 1500)
                {
                  DVRstatus = 0;
                }
            }
             
           
            if (DVRstatus == 1) 
            {
             u8g.drawBitmapP(96, 2, 1, 8, DVRstatus8_bitmap);
            }


            if(osdON == true)
            {
                u8g.setPrintPos(111, 9);
                u8g.print("OSD");
            }

            u8g.drawFrame(96, 16 - 1, 29, 20);
            u8g.drawBox(96, 16, 29, 6);
            u8g.setPrintPos(101, 31);
            u8g.print(alarmvalue);
            u8g.setColorIndex(0);
            u8g.setPrintPos(101, 21);
            u8g.setFont(u8g_font_micro);
            u8g.print("ALARM");
            u8g.setColorIndex(1);

            if (battery_health == 4) // case if voltage is 75% +
            {
                u8g.drawBox(2, 5, 1, 2);
                u8g.drawBox(4, 5, 1, 2);
                u8g.drawBox(6, 5, 1, 2);
                u8g.drawBox(8, 5, 1, 2);
                u8g.drawFrame(0, 5 - 2, 11, 6);
                u8g.drawBox(11, 5, 1, 2);
            }
            else if (battery_health == 3) // case if voltage is 50% +
            {
                u8g.drawBox(2, 5, 1, 2);
                u8g.drawBox(4, 5, 1, 2);
                u8g.drawBox(6, 5, 1, 2);
                u8g.drawFrame(0, 5 - 2, 11, 6);
                u8g.drawBox(11, 5, 1, 2);
            }
            else if (battery_health == 2) // case if voltage is 25% +
            {
                u8g.drawBox(2, 5, 1, 2);
                u8g.drawBox(4, 5, 1, 2);
                u8g.drawFrame(0, 5 - 2, 11, 6);
                u8g.drawBox(11, 5, 1, 2);
            }
            else if (battery_health == 1) // case if voltage is 5% +
            {

                u8g.drawBox(2, 5, 1, 2);
                u8g.drawFrame(0, 5 - 2, 11, 6);
                u8g.drawBox(11, 5, 1, 2);
            }
            else
            {
                u8g.drawFrame(0, 5 - 2, 11, 6);
                u8g.drawBox(11, 5, 1, 2);
            }

        }
        while (u8g.nextPage()
              );
        refreshi = 0;

    }

    buttoncheck();
    
    if (pressedbut == 1)
    {
        menu();
    }

    if (pressedbut == 4)
    {
        digitalWrite(DVR1_PIN, LOW);
        delay(480);
        digitalWrite(DVR1_PIN, HIGH);
        delay(5);
    }

    if (pressedbut == 3)
    {
        if (alarmvalue > 2.70)
        {
            alarmvalue -= 0.10;
            alarmvalueEEP = alarmvalue * 10.0;
            EEPROM.write(alarmADDR, alarmvalueEEP);
        }
    }

    if (pressedbut == 2)
    {
        if (alarmvalue < 3.90)
        {
            alarmvalue += 0.10;
            alarmvalueEEP = alarmvalue * 10.0;
            EEPROM.write(alarmADDR, alarmvalueEEP);
        }
    }


    delay(DELAY);

}



// Beep-Stuff


void beep_x(byte b)
{
    for (int i = 1; i <= b; i++)
    {
        tone(beeppin, note, 200);
        delay(400);
    }
}


void menu()
{
    menusel = 0;
    byte exit = 0;
    while(exit == 0)
    {
        clearOLED();
        u8g.firstPage();
        do
        {
            if(menusel == 0)
            {
                u8g.drawBitmapP(43, 8, 6, 35, DVR42_bitmap);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(45, 55);
                u8g.print("DVR MODE");

                u8g.drawDisc(115, 23, 2);
                u8g.drawCircle(115, 33, 2);
                u8g.drawCircle(115, 43, 2);

            }
            else if(menusel == 1)
            {
                u8g.drawBitmapP(43, 10, 6, 35, settings42_bitmap);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(44, 55);
                u8g.print("SETTINGS");

                u8g.drawCircle(115, 23, 2);
                u8g.drawDisc(115, 33, 2);
                u8g.drawCircle(115, 43, 2);
            }
            else if(menusel == 2)
            {
                u8g.drawBitmapP(43, 10, 6, 35, EXIT_bitmap);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(58, 55);
                u8g.print("EXIT");

                u8g.drawCircle(115, 23, 2);
                u8g.drawCircle(115, 33, 2);
                u8g.drawDisc(115, 43, 2);
            }
        }
        while (u8g.nextPage());

        pressedbut = buttoncheck();
        while(pressedbut == 0)
        {
            buttoncheck();
            ledcheck(); 
        }
        if(pressedbut == 1)
        {
            // Press selected Menu Point
            if(menusel == 2)
            {
                refreshi = 10;
                exit = 1;
            }
            if(menusel == 1)
            {
                refreshi = 10;
                exit = 1;
                submenu();
            }
            if(menusel == 0)
            {
                refreshi = 10;
                exit = 1;
                dvrmenu();
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

void dvrmenu()
{
    menusel = 0;
    byte exit = 0;
    while(exit == 0)
    {   
        clearOLED();
        u8g.firstPage();
    do
        {
                u8g.setPrintPos(1, 6);
                u8g.print("<PREV");
                u8g.setPrintPos(6, 14);
                u8g.print("REC");

                u8g.setPrintPos(53, 6);
                u8g.print("ENTER");
                u8g.setPrintPos(55, 14);
                u8g.print("MENU");
                
                u8g.setPrintPos(102, 6);
                u8g.print("NEXT>");
                u8g.setPrintPos(102, 14);
                u8g.print("PLAY");
                      
                
             if(menusel == 0) // first menu point
            {                
                u8g.drawBox(1, 20, 35, 16);
                u8g.setColorIndex(0);
                u8g.setColorIndex(1);

                u8g.drawFrame(47, 20, 35, 16);
                u8g.drawFrame(92, 20, 35, 16);

                u8g.drawFrame(1, 45, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(32, 56);
                u8g.print("EXIT DVR-MODE");
                u8g.setColorIndex(1);
            }
            
            else if(menusel == 1)
            {
                u8g.drawFrame(1, 20, 35, 16);

                u8g.drawBox(47, 20, 35, 16);
                u8g.setColorIndex(0);
                u8g.setColorIndex(1);

                u8g.drawFrame(92, 20, 35, 16);

                u8g.drawFrame(1, 45, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(32, 56);
                u8g.print("EXIT DVR-MODE");
                u8g.setColorIndex(1);
            }
            
            else if(menusel == 2)
            {
                u8g.drawFrame(1, 20, 35, 16);
                u8g.drawFrame(45, 20, 35, 16);

                u8g.drawBox(92, 20, 35, 16);
                u8g.setColorIndex(0);
                u8g.setColorIndex(1);

                u8g.drawFrame(1, 45, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(32, 56);
                u8g.print("EXIT DVR-MODE");
                u8g.setColorIndex(1);
            }

            else if(menusel == 3)
            {
                u8g.drawFrame(1, 20, 35, 16);
                u8g.drawFrame(45, 20, 35, 16);
                u8g.drawFrame(92, 20, 35, 16);

                u8g.drawBox(1, 45, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(32, 56);
                u8g.setColorIndex(0);
                u8g.print("EXIT DVR-MODE");
                u8g.setColorIndex(1);
            }
        }
        while (u8g.nextPage());

        pressedbut = buttoncheck();
        
          
        while(pressedbut == 0)
        {
            buttoncheck(); 
            ledcheck();  
        }
        if(pressedbut == 1) // Press selected Menu Point
        {

            if(menusel == 0)
            {    
                digitalWrite(DVR1_PIN, LOW);
                delay(480);
                digitalWrite(DVR1_PIN, HIGH);
                delay(5); 
            }
            if(menusel == 1)
            {
                digitalWrite(DVR2_PIN, LOW);
                delay(480);
                digitalWrite(DVR2_PIN, HIGH);
                delay(5);
            }
            if(menusel == 2)
            {
                digitalWrite(DVR3_PIN, LOW);
                delay(480);
                digitalWrite(DVR3_PIN, HIGH);
                delay(5);
            }
           
            if(menusel == 3)
            {
                refreshi = 10;
                exit = 1;
            }
        }
       
         else if(pressedbut == 4) // longpress button1
        {
            if(menusel == 0) {
             digitalWrite(DVR1_PIN, LOW);
             delay(1500);
             digitalWrite(DVR1_PIN, HIGH);
             delay(5);
            }
            if(menusel == 1) {
             digitalWrite(DVR2_PIN, LOW);
             delay(1500);
             digitalWrite(DVR2_PIN, HIGH);
             delay(5);
            }
             if(menusel == 2) {
             digitalWrite(DVR3_PIN, LOW);
             delay(1500);
             digitalWrite(DVR3_PIN, HIGH);
             delay(5);
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
            if(menusel < 3)
            {
                menusel++;
            }
        }
    }


}

void submenu()
{
    menusel = 0;
    byte exit = 0;
    while(exit == 0)
    {
        clearOLED();
        u8g.firstPage();
        do
        {
            if(menusel == 0) // first menu point
            {
                u8g.drawBox(1, 1, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setColorIndex(0);
                u8g.setPrintPos(20, 12);
                u8g.print("DVR AUTO-START");
                u8g.setPrintPos(105, 12);

                if(dvrEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(dvrEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 5, 2, 8, DVR_bitmap);
                u8g.setColorIndex(1);

                u8g.drawFrame(1, 16, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 27);
                u8g.print("OSD");
                u8g.setPrintPos(105, 27);

                if(layoutEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(layoutEEP == 1)
                {
                    u8g.print("ON");
                }
                else if(layoutEEP == 2)
                {
                    u8g.print("MIN");
                }

                u8g.drawBitmapP(5, 20, 2, 8, OSD_bitmap);


                u8g.drawFrame(1, 31, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 42);
                u8g.print("RSSI");
                u8g.setPrintPos(105, 42);


                if(rssiEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(rssiEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 35, 2, 8, RSSI_bitmap);


                u8g.drawFrame(1, 46, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 57);
                u8g.print("SAVE&EXIT");
                u8g.drawBitmapP(5, 49, 2, 8, back_bitmap);

            }
            else if(menusel == 1)
            {
                u8g.drawFrame(1, 1, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 12);
                u8g.print("DVR AUTO-START");
                u8g.setPrintPos(105, 12);

                if(dvrEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(dvrEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 5, 2, 8, DVR_bitmap);


                u8g.drawBox(1, 16, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setColorIndex(0);
                u8g.setPrintPos(20, 27);
                u8g.print("OSD");
                u8g.setPrintPos(105, 27);

                if(layoutEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(layoutEEP == 1)
                {
                    u8g.print("ON");
                }
                else if(layoutEEP == 2)
                {
                    u8g.print("MIN");
                }

                u8g.drawBitmapP(5, 20, 2, 8, OSD_bitmap);
                u8g.setColorIndex(1);


                u8g.drawFrame(1, 31, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 42);
                u8g.print("RSSI");
                u8g.setPrintPos(105, 42);

                if(rssiEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(rssiEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 35, 2, 8, RSSI_bitmap);

                u8g.drawFrame(1, 46, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 57);
                u8g.print("SAVE&EXIT");
                u8g.drawBitmapP(5, 49, 2, 8, back_bitmap);

            }
            else if(menusel == 2)
            {
                u8g.drawFrame(1, 1, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 12);
                u8g.print("DVR AUTO-START");
                u8g.setPrintPos(105, 12);

                if(dvrEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(dvrEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 5, 2, 8, DVR_bitmap);


                u8g.drawFrame(1, 16, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 27);
                u8g.print("OSD");
                u8g.setPrintPos(105, 27);

                if(layoutEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(layoutEEP == 1)
                {
                    u8g.print("ON");
                }
                else if(layoutEEP == 2)
                {
                    u8g.print("MIN");
                }

                u8g.drawBitmapP(5, 20, 2, 8, OSD_bitmap);

                u8g.drawBox(1, 31, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setColorIndex(0);
                u8g.setPrintPos(20, 42);
                u8g.print("RSSI");
                u8g.setPrintPos(105, 42);

                if(rssiEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(rssiEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 35, 2, 8, RSSI_bitmap);
                u8g.setColorIndex(1);

                u8g.drawFrame(1, 46, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 57);
                u8g.print("SAVE&EXIT");
                u8g.drawBitmapP(5, 49, 2, 8, back_bitmap);

            }

            else if(menusel == 3)
            {
                u8g.drawFrame(1, 1, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 12);
                u8g.print("DVR AUTO-START");
                u8g.setPrintPos(105, 12);

                if(dvrEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(dvrEEP == 1)
                {
                    u8g.print("ON");
                }


                u8g.setColorIndex(1);
                u8g.drawBitmapP(5, 5, 2, 8, DVR_bitmap);


                u8g.drawFrame(1, 16, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 27);
                u8g.print("OSD");
                u8g.setPrintPos(105, 27);

                if(layoutEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(layoutEEP == 1)
                {
                    u8g.print("ON");
                }
                else if(layoutEEP == 2)
                {
                    u8g.print("MIN");
                }

                u8g.drawBitmapP(5, 20, 2, 8, OSD_bitmap);

                u8g.drawFrame(1, 31, 126, 16);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 42);
                u8g.print("RSSI");
                u8g.setPrintPos(105, 42);

                if(rssiEEP == 0)
                {
                    u8g.print("OFF");
                }
                else if(rssiEEP == 1)
                {
                    u8g.print("ON");
                }

                u8g.drawBitmapP(5, 35, 2, 8, RSSI_bitmap);

                u8g.drawBox(1, 46, 126, 16);
                u8g.setColorIndex(0);
                u8g.setFont(u8g_font_5x7);
                u8g.setPrintPos(20, 57);
                u8g.print("SAVE&EXIT");
                u8g.drawBitmapP(5, 49, 2, 8, back_bitmap);
                u8g.setColorIndex(1);


            }
        }
        while (u8g.nextPage());

        pressedbut = buttoncheck();
        while(pressedbut == 0)
        {
            buttoncheck();
            ledcheck();

        }
        if(pressedbut == 1)
        {
            // Press selected Menu Point


            if(menusel == 0)
            {

                if(dvrEEP == 0)
                {
                    dvrEEP = 1;
                }
                else if(dvrEEP == 1)
                {
                    dvrEEP = 0;
                }

              //  Serial.print(dvrEEP);

            }

            if(menusel == 1)
            {

                if(layoutEEP == 0)
                {
                    layoutEEP = 1;
                }
                else if(layoutEEP == 1)
                {
                    layoutEEP = 2;
                }
                else if(layoutEEP == 2)
                {
                    layoutEEP = 0;
                }

                // Serial.print(layoutEEP);

            }

            if(menusel == 2)
            {

                if(rssiEEP == 0)
                {
                    rssiEEP = 1;
                }
                else if(rssiEEP == 1)
                {
                    rssiEEP = 0;
                }

                // Serial.print(rssiEEP);

            }


            if(menusel == 3)
            {

                EEPROM.write(layoutADDR, layoutEEP);
                EEPROM.write(dvrADDR, dvrEEP);
                EEPROM.write(rssiADDR, rssiEEP);
                refreshi = 10;
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
            if(menusel < 3)
            {
                menusel++;
            }
        }
    }


}
