#include "pong.h"

// TFT Instance
#define TFT_RST -1
#define TFT_CS   5
#define TFT_DC   6
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Seesaw Instance
Adafruit_miniTFTWing ss;

// // SD Card Instance
// #define SD_CS    4
// SdFat                SD;         // SD card filesystem
// Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
// Adafruit_Image       img;        // An image loaded into RAM
// int32_t              width  = 0; // BMP image dimensions
// int32_t              height = 0;

// Button Numbers
#define NONE    0
#define LEFT    1
#define RIGHT   2
#define DOWN    3
#define UP      4
#define A       5
#define B       6
#define SELECT  7

// Padle Structure
struct Paddle
{
  double x;
  double y;
  double speed;
  double angle;
  int width;
  int height;
};

// Ball Structure
struct Ball
{
  double x;
  double y;
  double speed;
  double angle;
  int radius;
};

struct Paddle player;
struct Paddle computer;
struct Ball ball;

// Score Variables
uint8_t score_player;
uint8_t score_computer;

// Button Events
uint8_t getButton()
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

void printTitle()
{
  // TODO: print title

  return;
}

uint8_t checkStart()
{
  uint8_t button = getButton();
  if(button == A)
  {
    return START;
  }
  return MENU;
}

void resetScores()
{
  score_player = 0;
  score_computer = 0;
}

void resetGame()
{
  player.x = PLAYER_START_X;
  player.y = PLAYER_START_Y;
  player.speed = PLAYER_DEFAULT_SPEED;

  computer.x = COMPUTER_START_X;
  computer.y = COMPUTER_START_Y;
  computer.speed = COMPUTER_DEFAULT_SPEED;

  ball.x = BALL_START_X;
  ball.y = BALL_START_Y;
  ball.speed = BALL_DEFAULT_SPEED;
  ball.angle = random(1,314) / 100;
}

void updatePlayer()
{
  // Get button press
  uint8_t button = getButton();

  // Up button
  if(button == UP)
  {
    // Move player up
    for(int n = 0; n < player.speed; n++)
    {
      // Check collision
      if(player.y - 1 > PLAYER_BOUNDS_LOWER)
      {
        player.y--;
      }
      else
      {
        break;
      }
    }
  }

  // Down button
  if(button == DOWN)
  {
    // Move player down
    for(int n = 0; n < player.speed; n++)
    {
      // Check collision
      if(player.y + 1 < PLAYER_BOUNDS_UPPER)
      {
        player.y++;
      }
      else
      {
        break;
      }
    }
  }
}

void updateComputer()
{
  // TODO: move computer
}

void updateBall()
{
  // Move ball according to angle
  for(int n = 0; n < ball.speed; n++)
  {
    // Move the ball vertically
    ball.y -= sin(ball.angle);

    // Check for wall collision
    if(((ball.y - ball.radius) <= BALL_BOUNDS_LOWER) ||
       ((ball.y + ball.radius) >= BALL_BOUNDS_UPPER))
    {
      ball.angle *= -1;
    }

    // Move the ball horizontally
    ball.x += cos(ball.angle);

    // Check for player collision
    if((ball.x - ball.radius) <= (player.x + player.width))
    {
      if((player.y <= ball.y ball.radius) && (ball.y <= player.y + player.height))
      {
        // Map ball position to placement on player paddle and compute angle
        ball.angle = ((ball.y - player.y) * (PI) / (player.height)) - HALF_PI;

        ball.angle *= -1; // Invert y-axis
      }
      else
      {
        break;
      }
    }

    // Check for computer collision
    if(ball.x >= computer.x)
    {
      if(computer.y <= ball.y && ball.y <= computer.y + computer.size)
      {
        // Map ball position to placement on computer paddle and compute angle
        ball.angle = ((ball.y - computer.y) * (PI) / (computer.height)) + HALF_PI;

        ball.angle *= -1; // Invert y-axis
      }
      else
      {
        break;
      }
    }
  }
}

void printGame()
{
  // TODO: print game and objects

  return;
}

void checkWin()
{
  // Check player loss
  if(ball.x <= player.x)
  {
    score_computer++;

    if(score_computer == MAX_SCORE)
    {
      return WIN;
    }
    else
    {
      resetGame();
    }
  }

  // Check computer loss
  if(ball.x >= computer.x)
  {
    score_player++;

    if(score_player == MAX_SCORE)
    {
      return WIN;
    }
    else
    {
      resetGame();
    }
  }

  return PLAY;
}

uint8_t getState(uint8_t state)
{
  switch(state)
  {
    case TITLE:
      printTitle();
      return MENU;

    case MENU:
      return checkStart();

    case START:
      resetScores();
      resetGame();
      return PLAY;

    case PLAY:
      updatePlayer();
      updateComputer();
      updateBall();
      printGame();
      return checkWin();

    case WIN:
      printWin();
      return MENU;
  }
}

void begin()
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

  // // Start SD Reader
  // ImageReturnCode stat;
  // while(!SD.begin(SD_CS, SD_SCK_MHZ(25)))
  // {
  //   Serial.println("Unable to start SD Reader...");
  //   delay(1000);
  // }

  // Start Random Number Generator
  randomSeed(analogRead(0));
}

void play()
{
  long time = millis();
  uint8_t state = 0;
  while (1)
  {
    if(millis() - time > 333)
    {
      state = getState(state);
      time = millis();
    }
  }
}
