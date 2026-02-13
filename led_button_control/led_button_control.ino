/*
 * LED Button Control for Arduino Uno with Grove Base Shield
 *
 * Hardware:
 * - Arduino Uno
 * - Grove Base Shield
 * - Grove LED Socket Module
 * - Grove Button Module
 *
 * Connections (Grove Base Shield):
 * - LED Socket: Connect to Grove port D4
 * - Button: Connect to Grove port D3
 *
 * Behavior:
 * - Press button to turn LED ON
 * - Release button to turn LED OFF
 */

// Pin definitions for Grove Base Shield
#define LED_PIN 4       // LED connected to Grove port D4
#define BUTTON_PIN 3    // Button connected to Grove port D3

// Variables
int buttonState = 0;    // Current state of the button

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("LED Button Control");
  Serial.println("==================");

  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Set button pin as input
  // Grove Button outputs HIGH when pressed, LOW when not pressed
  pinMode(BUTTON_PIN, INPUT);

  // Ensure LED is off at startup
  digitalWrite(LED_PIN, LOW);

  Serial.println("Ready! Press the button to light the LED.");
}

void loop() {
  // Read the current state of the button
  buttonState = digitalRead(BUTTON_PIN);

  // Check if button is pressed (HIGH for Grove Button module)
  if (buttonState == HIGH) {
    // Button is pressed - turn LED ON
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button PRESSED - LED ON");
  } else {
    // Button is not pressed - turn LED OFF
    digitalWrite(LED_PIN, LOW);
  }

  // Small delay to debounce
  delay(10);
}
