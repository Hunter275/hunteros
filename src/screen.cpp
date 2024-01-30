#include "screen.h"
#include <string>

using namespace hunteros;
using namespace std;

template <size_t N>
void screen::showMenuOptions(std::string (&menuItems)[N]) {
  for(std::string i : menuItems) {

  }
}

Adafruit_SSD1306 screen::getDisplay() {
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}