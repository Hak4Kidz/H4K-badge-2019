/*
    Hak4Kidz 2019 Tyro badge sketch.

    Version 0.5 of sketch was used for final code at Hak4Kidz Chicago.

    Contributors: Michael Whitely, Dave Schwartzberg, various Internet resources such as Adafruit.com.
    Handle: @compukidmike, @DSchwatzberg, @Hak4Kidz
*/

// declare included libraries
#include <Wire.h>
#include <pgmspace.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Red_October18pt7b.h"

// define constant values
#define Addr_VCC 0x78 //7 bit format is 0x3F;;
// LCD TFT Controllers
#define TFT_DC 5
#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_CLK 14
#define TFT_RST 18
// monochrome LEDs
#define D9  25  // left button
#define D10 26  // right button
#define D11 27  // First H4K chest light
#define D12 28
#define D13 29
#define D14 30
#define D15 31
#define D16 32  // Last H4K chest light

//drawing and font color constant values
#define ILI9341_BLACK 0x0000
#define ILI9341_BLUE  0x001F
#define ILI9341_PINK  0XEBD5
#define ILI9341_RED   0xF800
#define ILI9341_WHITE 0xFFFF

// assign temproary values to variables
int x;
int dim = 50;
int rotation = 3;
int threshold = 40;
int touchcounter = 0;
bool touch1detected = false;
bool touch2detected = false;
bool touch3detected = false;
bool touch4detected = false;
bool touch5detected = false;
bool touch6detected = false;

// a variable to hold a random number
long randomNumber;

// used to fade lights on and off
byte PWM_Gamma64[64] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x16,
  0x1a, 0x1c, 0x1d, 0x1f, 0x22, 0x25, 0x28, 0x2e,
  0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4b, 0x4f,
  0x55, 0x5a, 0x5f, 0x64, 0x69, 0x6d, 0x72, 0x77,
  0x7d, 0x80, 0x88, 0x8d, 0x94, 0x9a, 0xa0, 0xa7,
  0xac, 0xb0, 0xb9, 0xbf, 0xc6, 0xcb, 0xcf, 0xd6,
  0xe1, 0xe9, 0xed, 0xf1, 0xf6, 0xfa, 0xfe, 0xff
};

// assign the ILI9341 LCD to 'tft' variable
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);             // address the serial interface and tell it the speed
  Wire.begin(16, 17);               // tell MCU which pins to assdress the LED driver with
  Wire.setClock(400000);            // set I2C to 400kHz
  Wire.beginTransmission(60);       // start sending data to the LED driver
  Wire.write(0);                    //Register 0
  Wire.write(1);                    //Enable LED Driver PWM Value Register
  Wire.endTransmission();           // stop sending data to the LED driver


  // setting the interrupt pins
  Serial.println("Monitoring interrupts: ");
  //touchAttachInterrupt(0, gotTouch1, threshold);   // TP1 disabled due to USB short
  touchAttachInterrupt(4, gotTouch2, threshold);     // TP2
  touchAttachInterrupt(2, gotTouch3, threshold);     // TP3
  touchAttachInterrupt(27, gotTouch4, threshold);    // TP4
  touchAttachInterrupt(32, gotTouch5, threshold);    // TP5
  touchAttachInterrupt(33, gotTouch6, threshold);    // TP6

  Wire.beginTransmission(60);
  Wire.write(1);
  for (x = 0; x < 32; x++) {
    Wire.write(0);                   // Turn all LEDs off
  }
  for (x = 0; x < 4; x++) {
    Wire.write(127);                 // Turn LCD backlight on half brightness
  }
  Wire.write(0);                     //PWM Update
  for (x = 0; x < 36; x++) {
    Wire.write(1);                   // Turn all LED Controls to on, max current
  }
  Wire.endTransmission();

  randomSeed(digitalRead(0x00));     // used to reduce predictability.

  delay(1500);                       // give the TFT 1.5 secs to power up when running on battery
  tft.begin();                       // enable the LCD using tft
  tft.setRotation(3);                // change LCD orientation to be readable on badge
  // disabled until desired to use again
  // int x, y, w = tft.width(), h = tft.height();
  //for(y=0; y<10; y++) tft.drawFastHLine(0, y, w, ILI9341_WHITE);

  // prepare display for use
  tft.fillScreen(ILI9341_WHITE);               // clears tft to be white
  tft.setTextColor(ILI9341_BLACK);             // text will be written in black
  tft.println(" Booting ESP32 USB power / H4K 2019 Badge...\n");
  delay(1500);

  int x, y, w = tft.width(), h = tft.height();
  tft.fillRect(0, 0, w, 10, ILI9341_WHITE);    // clear USB Boot message
  tft.print("Initializing display... ");
  delay(300);
  tft.print("Done!\n\n\n");
  delay(1000);

  tft.print("Initializing illumination... ");
  delay(300);

  Init_FL3236A();                              // initialize the LED Driver
  tft.print("Done!\n\n\n");
  delay(300);

  tft.println("Initialization complete.\n\n");

  splash();                                    // who are we?
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  tft.fillScreen(ILI9341_BLACK);               // clears tft to be black
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(15, 100);
  tft.setTextSize(2);
  tft.println("Welcome to Hak4Kidz 2019");
  tft.setTextSize(3);
  tft.setCursor(120, 150);
  tft.println("TYRO");
  tft.setTextSize(2);

  LBlueEye();                                  // turn on the left eye LED
  RBlueEye();                                  // mturn on the right eye LED
  Buttons();                                   // turn on the button LEDs
  H4K();                                       // turn on the chest LEDs
  ArmPad();                                    // start the ArmPad function

  // checking for when a touch pad is touched.
  // this should be a case statement
  if (touch1detected && touchcounter > 1)
  {
    touch1detected = false;
    touchcounter = 0;
    Serial.println("Touch 1 detected");
  }

  if (touch2detected && touchcounter > 1)
  {
    touch2detected = false;
    touchcounter = 0;
    Serial.println("Touch 2 detected");
    myName();
  }

  if (touch3detected && touchcounter > 1)
  {
    touch3detected = false;
    touchcounter = 0;
    Serial.println("Touch 3 detected");
    lights();
  }

  if (touch4detected && touchcounter > 1)
  {
    touch4detected = false;
    touchcounter = 0;
    Serial.println("Touch 4 detected");
    secret();
  }

  if (touch5detected && touchcounter > 1)
  {
    touch5detected = false;
    touchcounter = 0;
    Serial.println("Touch 5 detected");
    draw();
  }

  if (touch6detected && touchcounter > 1)
  {
    touch6detected = false;
    touchcounter = 0;
    Serial.println("Touch 6 detected");
    donuts();
  }

  // tft.drawPixel(200, 200, ILI9341_PINK);                 // example how to draw a pink single pixel
}

// function to address LED driver to turn on LEDs
void IS_IIC_WriteByte(uint8_t Dev_Add, uint8_t Reg_Add, uint8_t Reg_Dat)
{
  Wire.beginTransmission(Dev_Add / 2);   // start transmitting
  Wire.write(Reg_Add);                   // sends regaddress
  Wire.write(Reg_Dat);                   // sends regaddress
  Wire.endTransmission();                // stop transmitting
}

// function to display the splash screen
void splash(void)
{
  tft.setFont(&Red_October18pt7b);   // set the font to Red Octopber
  tft.setTextSize(1);
  tft.setCursor(10, 130);
  delay(500);
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.print("H");
  tft.setTextSize(1);
  tft.print("ak");
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("4");
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.print("k");
  tft.setTextSize(1);
  tft.print("idz");
  delay(1000);
  tft.setFont(NULL);                 //reset to default font
}

// function to initialize the LED drivers and light show
void Init_FL3236A(void)
{
  uint8_t i = 0;

  //prepare ILI9341 for use
  tft.setFont(NULL);                          // reset to default font

  for (i = 0x26; i <= 0x49; i++)
  {
    IS_IIC_WriteByte(Addr_VCC, i, 0xff);      // turn  on  all  LED
  } delay(1000);

  for (i = 0x01; i <= 0x24; i++)
  {
    IS_IIC_WriteByte(Addr_VCC, i, 0x00);        // write  all  PWM  set  0x00
  }

  IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);     // update PWM & congtrol registers
  IS_IIC_WriteByte(Addr_VCC, 0x4B, 0x01);     // frequency  setting  22KHz
  IS_IIC_WriteByte(Addr_VCC, 0x00, 0x01);     // normal  operation

  IS31FL3236A_mode1();                        // breath of all the LEDs mode
}

void LBlueEye(void)
{
  IS_IIC_WriteByte(Addr_VCC, 0x03, 0xff);     //set  D1 blue
}

void RBlueEye(void)
{
  IS_IIC_WriteByte(Addr_VCC, 0x06, 0xff);     //set  D2 blue
}

void LRedEye(void)
{
  // set eyes red; not used
  IS_IIC_WriteByte(Addr_VCC, 0x01, 0xff);     //set  D1 red
}

void RRedEye(void)
{
  // set eyes red
  IS_IIC_WriteByte(Addr_VCC, 0x04, 0xff);     // set  D2 red
}

void Buttons(void)
{
  IS_IIC_WriteByte(Addr_VCC, 0x19, 0xff);     // set  D9  PWM
  IS_IIC_WriteByte(Addr_VCC, 0x1a, 0xff);     // set  D10  PWM
}

void H4K(void)
{
  int8_t h = 0;

  for (h = 0x1b; h < 0x21; h++)
  {
    IS_IIC_WriteByte(Addr_VCC, h, 0xff);     // set  D11 - D16  PWM
  }
}

void ArmPad(void)
{
  //uint8_t m = 0;
  int8_t n = 0;

  //assign a random number to variable
  randomNumber = random(7, 25);              // find a pseudo random number between 7 and 24

  Wire.beginTransmission(60);

  for (n = 0; n <= 63; n++)                   // all LED lights go on
  {
    // ArmPad RGB LEDs
    IS_IIC_WriteByte(Addr_VCC, randomNumber, PWM_Gamma64[n]); // set  random  PWM
  }
  IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);    // update  PWM  &  congtrol  registers
  delay(20);                                 // 20ms delay

   randomSeed(digitalRead(0x00));
   randomNumber = random(7, 25);              // find a pseudo random number between 7 and 24

  for (n = 63; n >= 0; n--)                  // all LED lights go off
  {
    IS_IIC_WriteByte(Addr_VCC, randomNumber, PWM_Gamma64[n]); //set  D9  PWM

    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00); // update  PWM  &  congtrol  registers
    delay(20);
  }
  Wire.endTransmission();
}

// this section is for the touch pad interrupt ISR
// this should really be a case statement
void gotTouch1()
{
  touch1detected = true;
  touchcounter++;
}

void gotTouch2()
{
  touch2detected = true;
  touchcounter++;
}

void gotTouch3()
{
  touch3detected = true;
  touchcounter++;
}
void gotTouch4()
{
  touch4detected = true;
  touchcounter++;
}
void gotTouch5()
{
  touch5detected = true;
  touchcounter++;
}

void gotTouch6()
{
  touch6detected = true;
  touchcounter++;
}

// a touch pad response
void myName()
{
  int y = random(0, 220);
  int x = random(0, 60);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_RED);

  tft.setCursor(y, x);
  tft.print("My name is Tinker!");
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
}

// a touch pad response
void donuts()
{
  int y = random(0, 220);
  int x = random(0, 300);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(y, x);
  tft.print("I love donuts!");
  delay(1000);

  // do it again!!
  y = random(0, 220);
  x = random(0, 300);
  tft.setCursor(y, x);
  tft.print("I love donuts!");
  tft.fillScreen(ILI9341_BLACK);
}

// a touch pad response
void draw()
{
  long start, t;
  int x1, y1, x2, y2,
      w = tft.width(),
      h = tft.height();

  int color = ILI9341_PURPLE;
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1 = y1 = 0;
  y2 = h - 1;
  start = micros();

  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t += micros() - start;

  yield();
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;
  start = micros();

  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_RED);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_RED);
  t += micros() - start;

  yield();
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  yield();

  y1 = h - 1;
  x1 = 0;
  y2 = 0;
  start = micros();

  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_CYAN);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_CYAN);
  t += micros() - start;

  yield();
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1 = w - 1;
  y2 = 0;
  y1 = h - 1;
  start = micros();

  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_GREEN);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, ILI9341_GREEN);
  t += micros() - start;

  yield();
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
}

// a touch pad response
void lights()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(40, 100);
  tft.println("Enjoy the light show!");
  for (int j = 63; j >= 0; j--)                    // all LED breath falling
  {
    for (int i = 1; i < 33; i++)
    {
      IS_IIC_WriteByte(Addr_VCC, i, PWM_Gamma64[j]); // fade out
    }
    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);         //update  PWM  &  congtrol  registers
    delay(50);
  }
  for (int j = 0; j <= 63; j++)                     //all LED fading out
  {
    IS_IIC_WriteByte(Addr_VCC, 0x19, PWM_Gamma64[j]); // set  D9  PWM
    IS_IIC_WriteByte(Addr_VCC, 0x1a, PWM_Gamma64[j]); // set  D10  PWM

    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);         // update  PWM  &  congtrol  registers
    delay(50);
  }

  // blink 3 times
  IS_IIC_WriteByte(Addr_VCC, D9, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D10, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D11, 0x00);
  delay(200);
  IS_IIC_WriteByte(Addr_VCC, D9, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D10, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D11, 0xff);
  delay(200);
  IS_IIC_WriteByte(Addr_VCC, D9, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D10, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D11, 0x00);
  delay(200);
  IS_IIC_WriteByte(Addr_VCC, D9, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D10, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D11, 0xff);
  delay(200);
  IS_IIC_WriteByte(Addr_VCC, D9, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D10, 0x00);
  IS_IIC_WriteByte(Addr_VCC, D11, 0x00);
  delay(200);
  IS_IIC_WriteByte(Addr_VCC, D9, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D10, 0xff);
  IS_IIC_WriteByte(Addr_VCC, D11, 0xff);

  Wire.endTransmission();
  Init_FL3236A();
  splash();
  delay(1000);
}

// a touch pad flag response
void secret()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(5, 10);
  tft.print("Bsdt gsjnztc wakz dkb ysxwdp. Bviv doqkth omhiiaofwlmiha.");
  delay(1500);
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.print("The key is on the cryptex.");
  delay(1000);
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
}

// run through all the LEDs and then fade out
void IS31FL3236A_mode1(void)
{
  uint8_t i = 0;
  int8_t j = 0;

  Wire.beginTransmission(60);

  for (i = 1; i < 13; i++)
  {
    IS_IIC_WriteByte(Addr_VCC, (i * 3 - 1), 0xff); // set PWM
    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);      // update PWM & congtrol  registers
    delay(100);
  }
  delay(50);

  for (i = 12; i > 0; i--)                       // LED  running
  {
    IS_IIC_WriteByte(Addr_VCC, (i * 3 - 2), 0xff); // set PWM
    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);      // update PWM & congtrol  registers
    delay(100);
  }
  delay(50);

  for (i = 1; i < 12; i++)                          // LED  running
  {
    IS_IIC_WriteByte(Addr_VCC, (i * 3 - 0), 0xff);  // set PWM
    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);         // update PWM & congtrol  registers
    delay(100);
  }
  delay(500);

  for (j = 63; j >= 0; j--)                         // all LED breath falling
  {
    for (i = 1; i < 33; i++)
    {
      IS_IIC_WriteByte(Addr_VCC, i, PWM_Gamma64[j]); // fade out
    }
    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);         //update  PWM  &  congtrol  registers
    delay(50);
  }
  for (j = 0; j <= 63; j++)                         // all LED breath falling
  {
    IS_IIC_WriteByte(Addr_VCC, 0x19, PWM_Gamma64[j]); // set  D9  PWM
    IS_IIC_WriteByte(Addr_VCC, 0x1a, PWM_Gamma64[j]); // set  D10  PWM

    IS_IIC_WriteByte(Addr_VCC, 0x25, 0x00);         // update PWM & congtrol  registers
    delay(50);
  }
  delay(500);
  Wire.endTransmission();
}
