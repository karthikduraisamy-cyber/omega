/*
 * Temperature and Humidity Monitor with Grove LCD Display
 *
 * Hardware:
 * - Arduino Uno
 * - DHT11 or DHT22 Temperature and Humidity Sensor
 * - Grove RGB LCD Display (16x2)
 *
 * Connections:
 * - DHT Sensor: Data pin to Digital Pin 2, VCC to 5V, GND to GND
 * - Grove LCD: Connect to I2C port (SDA to A4, SCL to A5)
 *
 * Required Libraries:
 * - DHT sensor library by Adafruit
 * - Grove LCD RGB Backlight library
 */

#include <Wire.h>
#include "rgb_lcd.h"
#include "DHT.h"

// DHT Sensor Configuration
#define DHTPIN 2          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT11 or DHT22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize Grove LCD
rgb_lcd lcd;

// LCD backlight colors
const int colorR = 0;
const int colorG = 128;
const int colorB = 64;

// Variables for sensor readings
float temperature = 0;
float humidity = 0;

// Previous readings for trend tracking
float prevTemperature = 0;
float prevHumidity = 0;
bool firstReading = true;

// Timing variables
unsigned long previousMillis = 0;
const long interval = 2000;  // Read sensor every 2 seconds

// Custom characters
byte degreeSymbol[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// Up arrow for rising values
byte upArrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

// Down arrow for falling values
byte downArrow[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

// Stable indicator (equals sign)
byte stableSymbol[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Temperature and Humidity Monitor");
  Serial.println("================================");

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD with 16 columns and 2 rows
  lcd.begin(16, 2);

  // Create custom characters
  lcd.createChar(0, degreeSymbol);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, stableSymbol);

  // Set LCD backlight color
  lcd.setRGB(colorR, colorG, colorB);

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Temp & Humidity");
  lcd.setCursor(0, 1);
  lcd.print("Monitor v1.0");

  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // Read sensor at specified interval
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Store previous readings for trend tracking
    if (!firstReading) {
      prevTemperature = temperature;
      prevHumidity = humidity;
    }

    // Read humidity and temperature
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();  // Celsius
    // For Fahrenheit: temperature = dht.readTemperature(true);

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Error: Failed to read from DHT sensor!");

      // Display error on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error!");
      lcd.setCursor(0, 1);
      lcd.print("Check connection");

      // Set LCD backlight to red for error
      lcd.setRGB(255, 0, 0);

      return;
    }

    // Reset backlight to normal color
    lcd.setRGB(colorR, colorG, colorB);

    // Print to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C ");
    Serial.print(getTrendChar(temperature, prevTemperature));
    Serial.print("  |  Humidity: ");
    Serial.print(humidity);
    Serial.print(" % ");
    Serial.println(getTrendChar(humidity, prevHumidity));

    // Mark first reading complete
    if (firstReading) {
      prevTemperature = temperature;
      prevHumidity = humidity;
      firstReading = false;
    }

    // Display on LCD
    displayReadings();

    // Change backlight color based on temperature
    updateBacklightColor();
  }
}

void displayReadings() {
  lcd.clear();

  // Display temperature on first row with trend arrow
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);  // 1 decimal place
  lcd.write((uint8_t)0);      // Custom degree symbol
  lcd.print("C");
  lcd.write(getTrendSymbol(temperature, prevTemperature));

  // Display humidity on second row with trend arrow
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humidity, 1);     // 1 decimal place
  lcd.print("%");
  lcd.write(getTrendSymbol(humidity, prevHumidity));
}

// Get trend symbol for LCD (custom character index)
uint8_t getTrendSymbol(float current, float previous) {
  float diff = current - previous;
  if (diff > 0.1) {
    return 1;  // Up arrow
  } else if (diff < -0.1) {
    return 2;  // Down arrow
  } else {
    return 3;  // Stable
  }
}

// Get trend character for Serial output
char getTrendChar(float current, float previous) {
  float diff = current - previous;
  if (diff > 0.1) {
    return '^';  // Rising
  } else if (diff < -0.1) {
    return 'v';  // Falling
  } else {
    return '=';  // Stable
  }
}

void updateBacklightColor() {
  // Change LCD backlight color based on temperature ranges
  if (temperature < 15) {
    // Cold - Blue
    lcd.setRGB(0, 0, 255);
  } else if (temperature < 25) {
    // Comfortable - Green
    lcd.setRGB(0, 255, 0);
  } else if (temperature < 30) {
    // Warm - Yellow
    lcd.setRGB(255, 255, 0);
  } else {
    // Hot - Red
    lcd.setRGB(255, 0, 0);
  }
}
