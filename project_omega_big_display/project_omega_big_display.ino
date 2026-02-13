/*
 * Project Omega - BIG Display with Flashy Animation
 *
 * Displays "PROJECT" and "OMEGA" in large letters across both lines
 * with flashy color-changing and animation effects
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
int animationPhase = 0;
int colorIndex = 0;
int flashCount = 0;

// Vibrant colors for effects (R, G, B)
const int numColors = 8;
const int colors[numColors][3] = {
  {255, 0, 0},     // Red
  {255, 128, 0},   // Orange
  {255, 255, 0},   // Yellow
  {0, 255, 0},     // Green
  {0, 255, 255},   // Cyan
  {0, 128, 255},   // Light Blue
  {128, 0, 255},   // Purple
  {255, 0, 255}    // Magenta
};

// Custom characters for big "P" letter
byte P_top_left[8] = {
  0b11111,
  0b11111,
  0b11000,
  0b11000,
  0b11111,
  0b11111,
  0b11000,
  0b11000
};

byte P_top_right[8] = {
  0b11111,
  0b11111,
  0b00011,
  0b00011,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

// Custom characters for big "Ω" (Omega) symbol
byte omega_top_left[8] = {
  0b00111,
  0b01111,
  0b11100,
  0b11000,
  0b11000,
  0b11000,
  0b11100,
  0b01110
};

byte omega_top_right[8] = {
  0b11100,
  0b11110,
  0b00111,
  0b00011,
  0b00011,
  0b00011,
  0b00111,
  0b01110
};

byte omega_bottom_left[8] = {
  0b00011,
  0b00001,
  0b00000,
  0b11111,
  0b11111,
  0b00000,
  0b00000,
  0b00000
};

byte omega_bottom_right[8] = {
  0b11000,
  0b10000,
  0b00000,
  0b11111,
  0b11111,
  0b00000,
  0b00000,
  0b00000
};

// Star/sparkle for effects
byte sparkle[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

// Block character for wipe effects
byte fullBlock[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

// Display strings - spread across both lines
const char* line1Text = "PROJECT * OMEGA ";  // Static top line (16 chars)
const char* line2Text = "   VIKRAM   *   AYAAN   *   SIDDHARTH   ";  // Scrolling names with spacing

// Scrolling variables
int scrollPosition = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 300;  // Time between scroll steps (ms)

// Forward declarations
void flashyDisplayLoop();
void introAnimation();
void typeText(int row, const char* text, int delayMs);
void sparkleEffect(int duration);
void bounceTransition();
void waveTransition();
void colorWipeTransition();
void dissolveTransition();
void slideTransition();
void curtainTransition();
void spiralTransition();
void flashBurstTransition();
void playTransition();
void oceanWaveEffect(unsigned long currentMillis, unsigned long &lastColorChange);
void auroraEffect(unsigned long currentMillis, unsigned long &lastColorChange);
void sunsetEffect(unsigned long currentMillis, unsigned long &lastColorChange);
void galaxyEffect(unsigned long currentMillis, unsigned long &lastColorChange);
void scrollLine2();


void setup() {
  Serial.begin(9600);
  Serial.println("Project Omega - BIG Display");
  Serial.println("============================");

  // Initialize LCD with 16 columns and 2 rows
  lcd.begin(16, 2);

  // Create custom characters
  lcd.createChar(0, sparkle);
  lcd.createChar(1, fullBlock);
  lcd.createChar(2, omega_top_left);
  lcd.createChar(3, omega_top_right);
  lcd.createChar(4, omega_bottom_left);
  lcd.createChar(5, omega_bottom_right);

  // Initial color - start dark
  lcd.setRGB(0, 0, 0);

  // Run intro animation
  introAnimation();
}

void loop() {
  // Main flashy animation loop
  flashyDisplayLoop();
}

// Epic intro animation - SLOWED DOWN
void introAnimation() {
  lcd.clear();

  // Phase 1: Color sweep from left to right
  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 0);
    lcd.write(byte(1));  // Full block
    lcd.setCursor(col, 1);
    lcd.write(byte(1));

    // Rainbow sweep
    int r = map(col, 0, 15, 255, 0);
    int g = map(col, 0, 15, 0, 255);
    int b = 128;
    lcd.setRGB(r, g, b);
    delay(100);
  }

  // Phase 2: Flash and clear
  for (int i = 0; i < 3; i++) {
    lcd.setRGB(255, 255, 255);
    delay(150);
    lcd.setRGB(0, 0, 0);
    delay(150);
  }

  lcd.clear();
  delay(400);

  // Phase 3: Type in "PROJECT * OMEGA" letter by letter on line 1
  lcd.setRGB(255, 0, 0);
  typeText(0, "PROJECT * OMEGA ", 100);

  // Flash between letters
  for (int i = 0; i < 2; i++) {
    lcd.setRGB(255, 255, 0);
    delay(200);
    lcd.setRGB(255, 0, 0);
    delay(200);
  }

  // Phase 4: Type in names on line 2
  lcd.setRGB(0, 255, 0);
  typeText(1, "VIKRAM AYAAN SID", 100);

  // Phase 5: Rainbow flash celebration
  for (int i = 0; i < 10; i++) {
    lcd.setRGB(colors[i % numColors][0], colors[i % numColors][1], colors[i % numColors][2]);
    delay(250);
  }

  // Phase 6: Sparkle effect
  sparkleEffect(1500);
}

// Type text character by character
void typeText(int row, const char* text, int delayMs) {
  lcd.setCursor(0, row);
  for (int i = 0; text[i] != '\0'; i++) {
    lcd.print(text[i]);
    delay(delayMs);
  }
}

// Scroll line 2 text
void scrollLine2() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastScrollTime >= scrollDelay) {
    lastScrollTime = currentMillis;

    int textLen = strlen(line2Text);

    // Display 16 characters starting from scrollPosition
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
      int charIndex = (scrollPosition + i) % textLen;
      lcd.print(line2Text[charIndex]);
    }

    // Move scroll position
    scrollPosition = (scrollPosition + 1) % textLen;
  }
}

// Sparkle effect with random sparkles - SLOWED DOWN
void sparkleEffect(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    int sparkleCol = random(0, 16);
    int sparkleRow = random(0, 2);

    // Add sparkle
    lcd.setCursor(sparkleCol, sparkleRow);
    lcd.write(byte(0));  // Sparkle character

    // Flash color
    lcd.setRGB(255, 255, 255);
    delay(100);

    // Restore - redraw the text
    lcd.setCursor(0, 0);
    lcd.print(line1Text);
    scrollLine2(); // Keep scrolling

    // Return to cycling color
    int idx = random(0, numColors);
    lcd.setRGB(colors[idx][0], colors[idx][1], colors[idx][2]);
    delay(100);
  }
}

// ============ TRANSITION ANIMATIONS ============

void bounceTransition() {
  for (int i = 0; i < 8; i++) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(line1Text);
    int r = map(i, 0, 7, 255, 0);
    int g = map(i, 0, 7, 0, 255);
    lcd.setRGB(r, g, 128);
    delay(100);
  }
  for (int i = 7; i >= 0; i--) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(line1Text);
    delay(80);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1Text);
}

void waveTransition() {
  for (int wave = 0; wave < 2; wave++) {
    for (int pos = 0; pos < 16; pos++) {
      lcd.setCursor(pos, 0);
      lcd.print(" ");
      lcd.setCursor(pos, 1);
      if (pos < strlen(line1Text)) lcd.print(line1Text[pos]);
      int r = (colors[wave % numColors][0] * pos) / 16;
      int g = (colors[(wave + 3) % numColors][1] * pos) / 16;
      int b = (colors[(wave + 5) % numColors][2] * pos) / 16;
      lcd.setRGB(r, g, b);
      delay(60);
    }
    delay(200);
    for (int pos = 0; pos < 16; pos++) {
      lcd.setCursor(pos, 0);
      if (pos < strlen(line1Text)) lcd.print(line1Text[pos]);
      delay(60);
    }
  }
}

void colorWipeTransition() {
  for (int i = 0; i < 16; i++) {
    int r = map(i, 0, 15, 255, 0);
    int g = map(i, 0, 15, 0, 255);
    int b = 128;
    lcd.setRGB(r, g, b);
    lcd.setCursor(i, 0);
    lcd.write(byte(0));
    lcd.setCursor(i, 1);
    lcd.write(byte(0));
    delay(90);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1Text);
}

void dissolveTransition() {
  bool dissolved[16][2] = {false};
  for (int count = 0; count < 24; count++) {
    int x = random(0, 16);
    int y = random(0, 2);
    if (!dissolved[x][y]) {
      dissolved[x][y] = true;
      lcd.setCursor(x, y);
      lcd.print(" ");
      lcd.setRGB(random(100, 255), random(100, 255), random(100, 255));
      delay(100);
    }
  }
  delay(500);
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 2; y++) {
      if (dissolved[x][y]) {
        lcd.setCursor(x, y);
        lcd.print(y == 0 ? line1Text[x] : ' ');
        lcd.setRGB(random(100, 255), random(100, 255), random(100, 255));
        delay(80);
      }
    }
  }
   lcd.setCursor(0, 0);
   lcd.print(line1Text);
}

void slideTransition() {
  for (int i = 0; i <= 16; i++) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(line1Text);
    lcd.setRGB(255 - (i * 15), i * 15, 128);
    delay(80);
  }
  for (int i = -16; i <= 0; i++) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(line1Text);
    lcd.setRGB(128, 255 + (i * 15), -i * 15);
    delay(80);
  }
}

void curtainTransition() {
  for (int i = 0; i <= 8; i++) {
    lcd.setCursor(i, 0);
    lcd.write(byte(1));
    lcd.setCursor(i, 1);
    lcd.write(byte(1));
    lcd.setCursor(15 - i, 0);
    lcd.write(byte(1));
    lcd.setCursor(15 - i, 1);
    lcd.write(byte(1));
    lcd.setRGB(colors[i % numColors][0], colors[i % numColors][1], colors[i % numColors][2]);
    delay(100);
  }
  delay(400);
  for (int i = 8; i >= 0; i--) {
    lcd.setCursor(i, 0);
    lcd.print(line1Text[i]);
    lcd.setCursor(15 - i, 0);
    lcd.print(line1Text[15 - i]);
    lcd.setRGB(colors[(8 - i) % numColors][0], colors[(8 - i) % numColors][1], colors[(8 - i) % numColors][2]);
    delay(120);
  }
}

void spiralTransition() {
  int positions[][2] = {
    {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0}, {7,0},
    {8,0}, {9,0}, {10,0}, {11,0}, {12,0}, {13,0}, {14,0}, {15,0},
    {15,1}, {14,1}, {13,1}, {12,1}, {11,1}, {10,1}, {9,1}, {8,1},
    {7,1}, {6,1}, {5,1}, {4,1}, {3,1}, {2,1}, {1,1}, {0,1}
  };
  for (int i = 0; i < 32; i++) {
    lcd.setCursor(positions[i][0], positions[i][1]);
    lcd.write(byte(0));
    lcd.setRGB(colors[i % numColors][0], colors[i % numColors][1], colors[i % numColors][2]);
    delay(60);
  }
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1Text);
}

void flashBurstTransition() {
  for (int burst = 0; burst < 4; burst++) {
    lcd.setRGB(255, 255, 255);
    delay(80);
    lcd.setRGB(colors[burst * 2][0], colors[burst * 2][1], colors[burst * 2][2]);
    delay(150);
    lcd.setRGB(0, 0, 0);
    delay(100);
  }
  for (int b = 0; b <= 255; b += 15) {
    lcd.setRGB(b, b, b);
    delay(40);
  }
}

void playTransition() {
  int transition = random(0, 8);
  switch (transition) {
    case 0: bounceTransition(); break;
    case 1: waveTransition(); break;
    case 2: colorWipeTransition(); break;
    case 3: dissolveTransition(); break;
    case 4: slideTransition(); break;
    case 5: curtainTransition(); break;
    case 6: spiralTransition(); break;
    case 7: flashBurstTransition(); break;
  }
}

// ============ DISPLAY EFFECTS ============

void oceanWaveEffect(unsigned long currentMillis, unsigned long &lastColorChange) {
  static int wave = 0;
  if (currentMillis - lastColorChange >= 100) {
    lastColorChange = currentMillis;
    int r = 0;
    int g = 100 + sin(wave * 0.05) * 80;
    int b = 150 + sin(wave * 0.03) * 100;
    lcd.setRGB(r, g, b);
    wave = (wave + 1) % 360;
  }
}

void auroraEffect(unsigned long currentMillis, unsigned long &lastColorChange) {
  static int shift = 0;
  if (currentMillis - lastColorChange >= 80) {
    lastColorChange = currentMillis;
    int r = sin(shift * 0.02) * 50 + 50;
    int g = sin(shift * 0.03 + 1) * 100 + 100;
    int b = sin(shift * 0.025 + 2) * 80 + 150;
    lcd.setRGB(r, g, b);
    shift = (shift + 1) % 500;
  }
}

void sunsetEffect(unsigned long currentMillis, unsigned long &lastColorChange) {
  static int phase = 0;
  if (currentMillis - lastColorChange >= 120) {
    lastColorChange = currentMillis;
    int r = 255 - (phase % 50);
    int g = 100 + sin(phase * 0.05) * 50;
    int b = sin(phase * 0.02) * 30;
    lcd.setRGB(r, g, b);
    phase = (phase + 1) % 200;
  }
}

void galaxyEffect(unsigned long currentMillis, unsigned long &lastColorChange) {
  static int phase = 0;
  if (currentMillis - lastColorChange >= 90) {
    lastColorChange = currentMillis;
    int r = sin(phase * 0.03) * 60 + 60;
    int g = 0;
    int b = sin(phase * 0.02 + 1) * 100 + 150;
    lcd.setRGB(r, g, b);
    phase = (phase + 1) % 400;
  }
}

// Main flashy display loop
void flashyDisplayLoop() {
  static unsigned long lastColorChange = 0;
  static unsigned long lastEffectChange = 0;
  static int currentEffect = 0;
  static float colorPosition = 0;
  static int pulseDirection = 1;
  static int pulseBrightness = 255;

  unsigned long currentMillis = millis();

  scrollLine2();

  if (currentMillis - lastEffectChange >= 10000) {
    lastEffectChange = currentMillis;
    playTransition();
    currentEffect = (currentEffect + 1) % 9;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1Text);
  }

  switch (currentEffect) {
    case 0:
      if (currentMillis - lastColorChange >= 120) {
        lastColorChange = currentMillis;
        colorPosition += 0.03;
        if (colorPosition >= numColors) colorPosition = 0;
        int idx = (int)colorPosition;
        int nextIdx = (idx + 1) % numColors;
        float blend = colorPosition - idx;
        int r = colors[idx][0] + (colors[nextIdx][0] - colors[idx][0]) * blend;
        int g = colors[idx][1] + (colors[nextIdx][1] - colors[idx][1]) * blend;
        int b = colors[idx][2] + (colors[nextIdx][2] - colors[idx][2]) * blend;
        lcd.setRGB(r, g, b);
      }
      break;
    case 1:
      if (currentMillis - lastColorChange >= 500) {
        lastColorChange = currentMillis;
        colorIndex = (colorIndex + 1) % numColors;
        lcd.setRGB(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
      }
      break;
    case 2:
      if (currentMillis - lastColorChange >= 50) {
        lastColorChange = currentMillis;
        pulseBrightness += pulseDirection * 4;
        if (pulseBrightness >= 255) {
          pulseBrightness = 255;
          pulseDirection = -1;
        } else if (pulseBrightness <= 30) {
          pulseBrightness = 30;
          pulseDirection = 1;
          colorIndex = (colorIndex + 1) % numColors;
        }
        float factor = pulseBrightness / 255.0;
        lcd.setRGB(colors[colorIndex][0] * factor, colors[colorIndex][1] * factor, colors[colorIndex][2] * factor);
      }
      break;
    case 3:
      if (currentMillis - lastColorChange >= 800) {
        lastColorChange = currentMillis;
        static bool toggle = false;
        toggle = !toggle;
        if (toggle) lcd.setRGB(255, 0, 255);
        else lcd.setRGB(0, 255, 255);
      }
      break;
    case 4:
      if (currentMillis - lastColorChange >= 180) {
        lastColorChange = currentMillis;
        int r = 200 + random(0, 56);
        int g = random(0, 150);
        int b = 0;
        lcd.setRGB(r, g, b);
      }
      break;
    case 5: oceanWaveEffect(currentMillis, lastColorChange); break;
    case 6: auroraEffect(currentMillis, lastColorChange); break;
    case 7: sunsetEffect(currentMillis, lastColorChange); break;
    case 8: galaxyEffect(currentMillis, lastColorChange); break;
  }

  static unsigned long lastTextEffect = 0;
  if (currentMillis - lastTextEffect >= 18000) {
    lastTextEffect = currentMillis;
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(i, 0);
      lcd.write(byte(0));
      lcd.setCursor(15 - i, 1);
      lcd.write(byte(0));
      delay(60);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1Text);
  }
}
