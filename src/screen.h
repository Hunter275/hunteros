#include <string>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

namespace hunteros
{
    class screen
    {
        public:
            //void clearBlue();
            // void clearYellow();
            // void writeYellow(String text, bool clear=true);
            // void writeYellow(String text, int x, int y, bool clear=true);
            // void writeBlue(String text, bool clear=true);
            // void writeBlue(String text, int x, int y, bool clear=true);
            // void drawMenuSkeleton();
            // void drawMenuItem(int position, String text);
            // void drawMenuItems(bool isUp, bool isFresh);
            // void drawBar();
            // void drawMenu(bool isUp=true, bool isFresh=false);
            // void showApplications();
            // void showSettings();
            // void showPower();
            // void goToScreen();
            template <size_t N>
            void showMenuOptions(Adafruit_SSD1306 display, std::string (&menuItems)[N]);
            //Adafruit_SSD1306 getDisplay(int width, int height, TwoWire &Wire, int reset);
            void drawMenuSkeleton(Adafruit_SSD1306 display);
    };
}