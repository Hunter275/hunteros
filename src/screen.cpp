#include "screen.h"
#include <string>
#include <Adafruit_SSD1306.h>

using namespace hunteros;
using namespace std;

template <size_t N>
void screen::showMenuOptions(Adafruit_SSD1306 display, std::string (&menuItems)[N]) {
  for(std::string i : menuItems) {

  }
}

// Adafruit_SSD1306 screen::getDisplay() {
//     Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// }