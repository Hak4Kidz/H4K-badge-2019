/* 
 *  Hak4Kidz 2019 badge test sketch.
 *  
 *  Basic sketch created to test the LEDs, LCD, and read Touch Pads for testing purposes only.
 *  
 *  Creator: Michael Whitely
 *  Handle: @compukidmike
 */
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 5
#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_CLK 14
#define TFT_RST 18

int x;
int dim = 50;
int rotation;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  // put your setup code here, to run once:
  Wire.begin(16, 17);
  Wire.beginTransmission(60);
  Wire.write(0); //Register 0
  Wire.write(1); //Enable LED Driver
  Wire.endTransmission();

  Wire.beginTransmission(60);
  Wire.write(1); //LED PWM Value Register
  for(x=0; x < 32; x++){
    Wire.write(0); //Turn all LEDs off
  }
  for(x=0; x < 4; x++){
    Wire.write(127); //Turn LCD backlight on half brightness
  }
  Wire.write(0); //PWM Update
  for(x=0; x < 36; x++){
    Wire.write(1); //Turn all LED Controls to on, max current
  }
  Wire.endTransmission();
  Serial.begin(115200);

  Serial.println("ILI9341 Test!"); 
 
  tft.begin();

  tft.fillScreen(ILI9341_BLACK);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(60);
  Wire.write(1); //LED PWM Value Register
  for(x=0; x < 32; x++){
    Wire.write(dim); //Set all LEDs to dim value
  }
  Wire.endTransmission();
  Wire.beginTransmission(60);
  Wire.write(0x25); //PWM Update Register
  Wire.write(0);
  Wire.endTransmission();

  Serial.print("1:");
  Serial.print(touchRead(0));
  Serial.print(" 2:");
  Serial.print(touchRead(4));
  Serial.print(" 3:");
  Serial.print(touchRead(2));
  Serial.print(" 4:");
  Serial.print(touchRead(27));
  Serial.print(" 5:");
  Serial.print(touchRead(32));
  Serial.print(" 6:");
  Serial.println(touchRead(33));

  
  int           x, y, w = tft.width(), h = tft.height();
  tft.fillRect(0, 0, w, 10, ILI9341_BLACK);

  //for(y=0; y<10; y++) tft.drawFastHLine(0, y, w, ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.print("1:");
  tft.print(touchRead(0));
  tft.print(" 2:");
  tft.print(touchRead(4));
  tft.print(" 3:");
  tft.print(touchRead(2));
  tft.print(" 4:");
  tft.print(touchRead(27));
  tft.print(" 5:");
  tft.print(touchRead(32));
  tft.print(" 6:");
  tft.println(touchRead(33));
  delay(100);

}
