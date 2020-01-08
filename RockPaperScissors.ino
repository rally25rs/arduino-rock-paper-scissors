#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeMonoBold12pt7b.h> 
#include <Fonts/FreeSansBold18pt7b.h> 
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp32_badge.h"

#define BADGE_WIFI_MODE WIFI_MODE_AP // use WIFI_MODE_STA (station) for home WiFi, WIFI_MODE_AP (access point) when using mobile hotspot
#define COLOR_SCREEN_BGD          ILI9341_WHITE
#define COLOR_SCREEN_TEXT         ILI9341_BLACK

const char*    ssid     = "codemash"; // TODO: add your network ID here 
const char*    password = ""; // TODO: add your network password here 
const char*    game_api_url = "https://api.weather.gov/gridpoints/CLE/48,56/forecast";
const uint16_t net_connect_timeout_sec = 10;
const uint16_t find_opponents_refresh_sec = 10;
bool           wifi_available = false;
char*          player_name = "Jeff"; // TODO: This would be editable in MODE_SETUP_GAME

bool btnA_pressed, btnB_pressed, btnX_pressed, btnY_pressed;
bool btnUp_pressed, btnDown_pressed, btnLeft_pressed, btnRight_pressed;
bool btnUp_released, btnDown_released, btnLeft_released, btnRight_released;
bool spkr_on, led1_on, led2_on, led3_on;

enum display_mode_type 
{
  MODE_SETUP_GAME,
  MODE_FIND_OPPONENT,
  MODE_PLAY_GAME,
  MODE_GAME_RESULT
};
enum display_mode_type display_mode;
display_mode_type display_mode = MODE_SETUP_GAME

bool connectToWiFi(uint16_t timeout_sec)
{
  uint16_t elapsed_sec = 0;

  Serial.printf("Connecting to %s ", ssid);
  digitalWrite(LED_1, LOW);  // reset error indicator

  WiFi.mode(BADGE_WIFI_MODE);
  delay(500);
  WiFi.begin(ssid, password);
  
  while ((WiFi.status() != WL_CONNECTED) && (elapsed_sec < timeout_sec)) 
  {
    digitalWrite(LED_2, HIGH);
    delay(500);
    Serial.print(".");
    digitalWrite(LED_2, LOW);
    delay(500);
    elapsed_sec += 1;
  }
  Serial.println();
  
  //if (WiFi.status() != WL_CONNECTED)
  if (elapsed_sec >= timeout_sec)
  {
    digitalWrite(LED_1, HIGH); // red=problem 
    Serial.println("Network connection timeout.");
    wifi_available = false;
    return false;  // failure 
  }

  wifi_available = true;
  return true; // success
}

void errorWithMessage(char* message) {
  digitalWrite(LED_1, HIGH); // red=problem 
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  tft.setFont(&FreeSansBold9pt7b);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 5);
  tft.print(message);
  while(true) delay(1);
}

void updateSetupGameView() {
  tft.fillScreen(COLOR_SCREEN_BGD);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 RockPaperScissors : Starting Up"); 
  delay(100);

  // Set up user input pins
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);   
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);

  // Set up the speaker and LED output pins
  pinMode(SPKR, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  delay(100);
  
  // Set up the TFT screen
  tft.begin();
  tft.setRotation(SCREEN_ROT);
}

void loop() {
  switch (display_mode)
  {
    case MODE_SETUP_GAME: 
      updateSetupGameView();
      break;

    default:
      display_mode = MODE_SETUP_GAME;
      break;    
  }
}
