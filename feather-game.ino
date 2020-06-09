// Graphics Library
#include <Adafruit_GFX.h>

// TFT Driver Library
#include <Adafruit_ST7735.h>

// Seesaw Interface Library
#include <Adafruit_miniTFTWing.h>

// SD Card & FAT Filesystem Library
#include <SdFat.h>

// SD Card Image Reading Library
#include <Adafruit_ImageReader.h>

// TFT Instance
#define TFT_RST -1
#define TFT_CS   5
#define TFT_DC   6
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Seesaw Instance
Adafruit_miniTFTWing ss;

// SD Card Instance
#define SD_CS    4
SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0; // BMP image dimensions
int32_t              height = 0;

// Other Items
#define NONE    0
#define LEFT    1
#define RIGHT   2
#define DOWN    3
#define UP      4
#define A       5
#define B       6
#define SELECT  7

#define GET_NEW_EVENT 100


// Setup Function
void setup()
{
  // Start Serial module
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Serial initialized!");

  // Start Seesaw module
  while(!ss.begin())
  {
    Serial.println("Unable to start Seesaw...");
    delay(1000);
  }
  ss.tftReset();
  ss.setBacklight(TFTWING_BACKLIGHT_ON);
  Serial.println("Seesaw initialized!");

  // Start TFT screen
  tft.initR(INITR_MINI160x80);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  Serial.println("TFT initialized!");

  // Start SD Reader
  ImageReturnCode stat;
  while(!SD.begin(SD_CS, SD_SCK_MHZ(25))) 
  {
    Serial.println("Unable to start SD Reader...");
    delay(1000);
  }
}

// Button Events
uint8_t getEvent()
{
  uint32_t buttons = ss.readButtons();
  if(!(buttons & TFTWING_BUTTON_LEFT))    {return LEFT;}
  if(!(buttons & TFTWING_BUTTON_RIGHT))   {return RIGHT;}
  if(!(buttons & TFTWING_BUTTON_DOWN))    {return DOWN;}
  if(!(buttons & TFTWING_BUTTON_UP))      {return UP;}
  if(!(buttons & TFTWING_BUTTON_A))       {return A;}
  if(!(buttons & TFTWING_BUTTON_B))       {return B;}
  if(!(buttons & TFTWING_BUTTON_SELECT))  {return SELECT;}
  /* there are no buttons pressed */      {return NONE;}
}

// Main Loop
void loop()
{
  static uint8_t event = NONE;
  
  static long last_event = millis();
  if(millis() - last_event > GET_NEW_EVENT)
  {
    long start = micros();
    event = getEvent();
    long finish = micros();

    Serial.print("Event Period: "); Serial.print(finish-start); Serial.println("us");
  }
}
