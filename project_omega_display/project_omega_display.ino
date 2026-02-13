/*
 * Project Omega - Autonomous Car Display
 *
 * Flashy animated display using Grove RGB LCD
 * Shows "PROJECT OMEGA" and "AUTONOMOUS CAR" with color-changing effects
 *
 * Hardware:
 * - Arduino Uno
 * - Grove RGB LCD Display (16x2)
 *
 * Connections:
 * - Grove LCD: Connect to I2C port (SDA to A4, SCL to A5)
 *
 * Required Libraries:
 * - Grove LCD RGB Backlight library
 */

#include <Wire.h>
#include "rgb_lcd.h"

// Initialize Grove LCD
rgb_lcd lcd;

// Animation timing
unsigned long previousMillis = 0;
const int colorChangeInterval = 100;  // Color change speed (ms)

// Color variables
int currentColorIndex = 0;
int brightness = 255;
int brightnessDirection = -5;

// Predefined vibrant colors for cycling (R, G, B)
const int numColors = 8;
const int colors[numColors][3] = {
  {255, 255, 255}, // White
  {255, 128, 0},   // Orange
  {255, 255, 0},   // Yellow
  {0, 255, 0},     // Green
  {0, 255, 255},   // Cyan
  {0, 0, 255},     // Blue
  {128, 0, 255},   // Purple
  {255, 0, 128}    // Pink
};

// Display strings
const char* line1 = "PROJECT OMEGA";
// Scrolling text - use \x04 as placeholder for omega character (custom char 4)
const char line2[] = " \x04 VIKRAM KARTHIK \x04 AYAAN \x04 SIDDHARTH \x04 ";
const int line2TotalLen = sizeof(line2) - 1;  // Automatically calculate length

// Custom characters for effects
byte block[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte leftArrow[8] = {
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000
};

byte rightArrow[8] = {
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000
};

byte star[8] = {
  0b00000,
  0b00100,
  0b10101,
  0b01110,
  0b01110,
  0b10101,
  0b00100,
  0b00000
};

byte omega[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01010,
  0b11011,
  0b00000
};

void setup() {
  Serial.begin(9600);
  Serial.println("Project Omega Display");
  Serial.println("=====================");

  // Initialize LCD with 16 columns and 2 rows
  lcd.begin(16, 2);

  // Create custom characters
  lcd.createChar(0, block);
  lcd.createChar(1, leftArrow);
  lcd.createChar(2, rightArrow);
  lcd.createChar(3, star);
  lcd.createChar(4, omega);

  // Initial color
  lcd.setRGB(255, 0, 0);

  // Startup animation - wipe in effect
  startupAnimation();
}

void loop() {
  rainbowCycleAnimation();
}

// Startup wipe-in animation
void startupAnimation() {
  lcd.clear();
  lcd.setRGB(0, 0, 0);

  int line1Len = strlen(line1);  // 15 chars with spaces

  // Character-by-character reveal with color fade in
  // Line 1 format: ◄ PROJECT OMEGA ► (arrow + 15 chars + arrow)
  // Line 2 format: VIKRAM   AYAAN   (first 16 chars of scrolling text)

  int maxLen = 16;
  for (int i = 0; i < maxLen; i++) {
    // Set color gradient
    int r = map(i, 0, 15, 0, 255);
    int g = map(i, 0, 15, 255, 0);
    int b = 128;
    lcd.setRGB(r, g, b);

    // Line 1: Arrow at 0, text from 1-15
    if (i == 0) {
      lcd.setCursor(0, 0);
      lcd.write(byte(1));  // Left arrow
    } else if (i <= line1Len && i <= 14) {
      lcd.setCursor(i, 0);
      lcd.print(line1[i - 1]);
    } else if (i == 15) {
      lcd.setCursor(i, 0);
      lcd.write(byte(2));  // Right arrow
    }

    // Line 2: Show first 16 characters of scrolling names
    if (i < 16) {
      lcd.setCursor(i, 1);
      lcd.print(line2[i]);
    }

    delay(100);
  }

  // Flash effect at the end
  for (int i = 0; i < 3; i++) {
    lcd.setRGB(255, 255, 255);
    delay(100);
    lcd.setRGB(0, 255, 255);
    delay(100);
  }
}

// Rainbow color cycling with scrolling names
void rainbowCycleAnimation() {
  static unsigned long lastColorChange = 0;
  static unsigned long lastScroll = 0;
  static int scrollPos = 0;
  unsigned long currentMillis = millis();

  // Display line 1 with arrows (only once)
  static bool line1Displayed = false;
  if (!line1Displayed) {
    lcd.clear();
    // Line 1: ◄ PROJECT OMEGA ►
    lcd.setCursor(0, 0);
    lcd.write(byte(1));  // Left arrow
    lcd.print(line1);
    lcd.write(byte(2));  // Right arrow
    line1Displayed = true;
  }

  // Scroll line 2 (names) from right to left
  if (currentMillis - lastScroll >= 300) {  // Scroll speed
    lastScroll = currentMillis;

    lcd.setCursor(0, 1);
    // Display 16 characters from the scrolling text
    for (int i = 0; i < 16; i++) {
      int charIndex = (scrollPos + i) % line2TotalLen;
      lcd.print(line2[charIndex]);
    }

    scrollPos++;
    if (scrollPos >= line2TotalLen) {
      scrollPos = 0;
    }
  }

  // Cycle through colors smoothly
  if (currentMillis - lastColorChange >= colorChangeInterval) {
    lastColorChange = currentMillis;

    // Interpolate between colors for smooth transition
    static float colorPosition = 0;
    colorPosition += 0.1;
    if (colorPosition >= numColors) colorPosition = 0;

    int currentIdx = (int)colorPosition;
    int nextIdx = (currentIdx + 1) % numColors;
    float blend = colorPosition - currentIdx;

    int r = colors[currentIdx][0] + (colors[nextIdx][0] - colors[currentIdx][0]) * blend;
    int g = colors[currentIdx][1] + (colors[nextIdx][1] - colors[currentIdx][1]) * blend;
    int b = colors[currentIdx][2] + (colors[nextIdx][2] - colors[currentIdx][2]) * blend;

    lcd.setRGB(r, g, b);
  }
}

