#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <iostream>


HTTPClient http;
WiFiMulti wfm;
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"pool.ntp.org", -18000, 60000);

String app = "hunteros";
#define URL "http://192.168.1.16/log"

// BUTTONS
#define UP_BUTTON 25
#define DOWN_BUTTON 26
#define LEFT_BUTTON 27
#define RIGHT_BUTTON 2
#define ENTER_BUTTON 32
#define BACK_BUTTON 33

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define black SSD1306_BLACK
#define white SSD1306_WHITE

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String incoming = "";
unsigned long lastInputEpoch = 0;

String menus[4] = {"Information", "Applications", "Settings", "Screen Sleep"};

int menu_index = 0;

// INFO:
// 21 characters per line
// 2 lines of yellow
// 6 lines of blue

int UP_BUTTON_last_state = HIGH;
int UP_BUTTON_state;
int DOWN_BUTTON_last_state = HIGH;
int DOWN_BUTTON_state;
int LEFT_BUTTON_last_state = HIGH;
int LEFT_BUTTON_state;
int RIGHT_BUTTON_last_state = HIGH;
int RIGHT_BUTTON_state;
int ENTER_BUTTON_last_state = HIGH;
int ENTER_BUTTON_state;
int BACK_BUTTON_last_state = HIGH;
int BACK_BUTTON_state;

bool isSleeping = false;

void log(String text) {
  Serial.println(text);
}

void GetUrl(String url) {
  String result = "nothing";
  log("GetUrl(" + url + ") called.");
  http.begin(url);
  log("Sending request");
  http.GET();
  log("Request sent");
  result = http.getString();
  log("Result " + result);
  log("Tearing down http");
  http.end();
}

void PostUrl(String url, String body) {
  log("PostUrl() called");
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  //http.addHeader("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiIyNTE1ZTE4NTczYTk0MDJhOTBlZGZmYzdmY2JhNmRmZiIsImlhdCI6MTcwNDA0ODYzNiwiZXhwIjoyMDE5NDA4NjM2fQ.M9Zk58edToDMyXE_4FCTSoC0Wo0S9SkFwBhKpW_5Rx8");
  log("Sending request");
  log("Body " + body);
  int code = http.POST(body);
  log("Code " + code);
  log(http.getString());
  log("Tearing down POST");
  http.end();
}

void clearBlue() {
  display.fillRect(0, 16, SCREEN_WIDTH, 50, SSD1306_BLACK);
}

void clearYellow() {
  display.fillRect(0, 0, SCREEN_WIDTH, 16, SSD1306_WHITE);
}

void writeYellow(String text, bool clear=true) {
  int str_len = text.length() + 1;
  char char_array[str_len];
  text.toCharArray(char_array, str_len);
  if (clear) {
    clearYellow();
    delay(100);
  }
  display.setCursor(0,0);
  display.write(char_array);
  display.display();
  delay(1);
}

void writeYellow(String text, int x, int y, bool clear=true) {
  int str_len = text.length() + 1;
  char char_array[str_len];
  text.toCharArray(char_array, str_len);
  if (clear) {
    clearYellow();
    delay(100);
  }
  display.setCursor(x,y);
  display.setTextColor(black);
  display.write(char_array);
  display.setTextColor(white);
  display.display();
  delay(1);
}

void writeBlue(String text, bool clear=true) {
  int str_len = text.length() + 1;
  char char_array[str_len];
  text.toCharArray(char_array, str_len);
  if (clear) {
    clearBlue();
    delay(100);
  }
  delay(1);
  display.setCursor(0,16);
  display.write(char_array);
  display.display();
  delay(1);
}

void writeBlue(String text, int x, int y, bool clear=true) {
  y = y + 16; // offset y value to start 0,0 in blue section only
  int str_len = text.length() + 1;
  char char_array[str_len];
  text.toCharArray(char_array, str_len);
  if (clear) {
    clearBlue();
    delay(100);
  }
  delay(1);
  display.setCursor(x,y);
  display.write(char_array);
  display.display();
  delay(1);
}

void drawMenuSkeleton() {
  display.drawRoundRect(0, 16, 128, 12, 3, SSD1306_WHITE);
  display.fillRoundRect(0, 30, 128, 10, 3, white);
  display.drawRoundRect(0, 42, 128, 12, 3, SSD1306_WHITE);
}

void drawMenuItem(int position, String text) {
  if (position == 0) {
    writeBlue(text, 5, 2, false);
  }

  if (position == 1) {
    display.setTextColor(black);
    writeBlue(text, 5, 16, false);
    display.setTextColor(white);
  }

  if (position == 2) {
    writeBlue(text, 5, 28, false);
  }
}

void drawMenuItems(bool isUp, bool isFresh) {
  if (!isFresh) {
    log("isUp");
    Serial.println(isUp);
    log("Menu Index");
    Serial.println(menu_index);
    if (!isUp) { // this is inverted because of how scrolling works
      if (menu_index == 3) {
        log("Setting index to 0");
        menu_index = 0;
      } else {
        log("Increasing index by 1");
        menu_index++;
      }
    }
    else {
      if (menu_index == 0) {
        log("Setting index to 3");
        menu_index = 3;
      } else {
        log("Decreasomg index by 1");
        menu_index--;
      }
    }
  }

  // draw previous
  if (menu_index == 0) {
    drawMenuItem(0, menus[3]);
  }
  else {
    drawMenuItem(0, menus[menu_index - 1]);
  }

  // draw selected
  drawMenuItem(1, menus[menu_index]);

  // draw next
  if (menu_index == 3) {
    drawMenuItem(2, menus[0]);
  }
  else {
    drawMenuItem(2, menus[menu_index + 1]);
  }
}

void goToSleep() {
  isSleeping = true;
  display.clearDisplay();
  display.display();
}

void drawBar(bool isFresh=false) {
  if (!isSleeping) {
    if (isFresh) {
      log("Fresh time, updating");
      timeClient.update();
      delay(3000);
    }
    log("Not sleeping");
    display.drawRect(0, 0, 128, 16, SSD1306_WHITE);

    unsigned long rawTime = timeClient.getEpochTime();
    //rawTime = rawTime - 3600; 
    unsigned long hours = (rawTime % 86400L) / 3600;
    unsigned long minutes = (rawTime % 3600) / 60;
    unsigned long seconds = rawTime % 60;

    String hour = hours > 12 ? String(hours - 12) : String(hours);
    hour = hours < 10 ? "0" + hour : hour;
    hour = hour == "00" ? "12" : hour;
    String minute = String(minutes);
    minute = minutes < 10 ? "0" + minute : minute;
    String second = String(seconds);
    second = seconds < 10 ? "0" + second : second;
    String ampm = hours > 12 ? "pm" : "am";

    String barString = hour + ":" + minute + ":" + second + " " + ampm;
    writeYellow(barString, 45, 5);
    // if ((rawTime - lastInputEpoch) > 300 && !isFresh) {
    //   Serial.println(rawTime);
    //   Serial.println(lastInputEpoch);
    //   Serial.println(rawTime - lastInputEpoch);
    //   log("Too much time has passed, sleeping");
    //   goToSleep();
    // }
  }
}

void drawMenu(bool isUp=true, bool isFresh=false) {
  isSleeping = false;
  timeClient.update();
  display.clearDisplay();
  drawBar(isFresh);
  drawMenuSkeleton();
  drawMenuItems(isUp, isFresh);
}

void showApplications() {
  clearBlue();
  writeBlue("This is the applications page.");
  display.display();
  PostUrl(URL, "{\"source\":\"5minheartbeat\", \"level\":\"info\", \"message\":\"Hello, world!\"}");
}
void showSettings() {
  clearBlue();
  writeBlue("This is the settings page.");
  display.display();
}

void showInformation() {
  String ip_address = "IP: " + WiFi.localIP().toString();
  String ssid = "SSID: " + String(WiFi.SSID());
  String strength = "Strength: " + String(WiFi.RSSI());
  clearBlue();
  writeBlue(ip_address, 0, 4, false);
  writeBlue(ssid, 0, 18, false);
  writeBlue(strength, 0, 32, false);
}

void goToScreen() {
  log("goToScreen called");
  Serial.println(menu_index);
  switch (menu_index) {
    case 0:
      showInformation();
      break;
    case 1:
      showApplications();
      break;
    case 2:
      showSettings();
      break;
    case 3:
      goToSleep();
      break;
    default:
      drawMenu();
      break;
  }
}

void initWiFi() {
  log("Connecting to WiFi");
  //writeBlue("Connecting to WiFi...");
  wfm.addAP("Wintermute", "andrew2373");
  while(wfm.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println(WiFi.localIP());
}

uint32_t startupMillis;

void resetSleepTimer() {
  unsigned long t = timeClient.getEpochTime();
  log("Setting lastInputEpoch to " + t);
  lastInputEpoch = t;
}

void setup() {
  startupMillis = millis();
  // while(millis() - startupMillis < 60000)
  // {
  //   drawBar();
  // }
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(ENTER_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  initWiFi();
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  timeClient.begin();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  writeYellow("WELCOME TO", 33, 5);
  writeBlue("HunterOS", 38, 10);
  writeBlue("v0.1", 50, 20, false);
  log("Booted");
  delay(1);
  drawMenu(false, true);
}


void loop() {
  if (millis() - startupMillis >=  1000) {
    startupMillis = millis();
    drawBar();
  }
  UP_BUTTON_state = digitalRead(UP_BUTTON);
  DOWN_BUTTON_state = digitalRead(DOWN_BUTTON);
  LEFT_BUTTON_state = digitalRead(LEFT_BUTTON);
  RIGHT_BUTTON_state = digitalRead(RIGHT_BUTTON);
  ENTER_BUTTON_state = digitalRead(ENTER_BUTTON);
  BACK_BUTTON_state = digitalRead(BACK_BUTTON);

  if(UP_BUTTON_last_state == LOW && UP_BUTTON_state == HIGH) {
    Serial.println("UP_BUTTON pressed");
    resetSleepTimer();
    drawMenu();
  }
  if(DOWN_BUTTON_last_state == LOW && DOWN_BUTTON_state == HIGH) {
    Serial.println("DOWN_BUTTON pressed");
    resetSleepTimer();
    drawMenu(false);
  }
  if(LEFT_BUTTON_last_state == LOW && LEFT_BUTTON_state == HIGH) {
    Serial.println("LEFT_BUTTON pressed");
    resetSleepTimer();
  }
  if(RIGHT_BUTTON_last_state == LOW && RIGHT_BUTTON_state == HIGH) {
    Serial.println("RIGHT_BUTTON pressed");
    resetSleepTimer();
  }
  if(ENTER_BUTTON_last_state == LOW && ENTER_BUTTON_state == HIGH) {
    Serial.println("ENTER_BUTTON pressed");
    resetSleepTimer();
    goToScreen();
  }
  if(BACK_BUTTON_last_state == LOW && BACK_BUTTON_state == HIGH) {
    Serial.println("BACK_BUTTON pressed");
    resetSleepTimer();
  }
  UP_BUTTON_last_state = UP_BUTTON_state;
  DOWN_BUTTON_last_state = DOWN_BUTTON_state;
  LEFT_BUTTON_last_state = LEFT_BUTTON_state;
  RIGHT_BUTTON_last_state = RIGHT_BUTTON_state;
  ENTER_BUTTON_last_state = ENTER_BUTTON_state;
  BACK_BUTTON_last_state = BACK_BUTTON_state;
}

