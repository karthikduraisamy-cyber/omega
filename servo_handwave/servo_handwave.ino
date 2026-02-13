/*
 * Servo Handwave for Arduino Uno with Grove Base Shield
 *
 * Hardware:
 * - Arduino Uno
 * - Grove Base Shield
 * - Servo Motor (connected via Grove Servo cable or directly)
 *
 * Connections (Grove Base Shield):
 * - Servo: Connect to Grove port D5 (or use servo signal wire to pin 5)
 *
 * Behavior:
 * - Starts at 0 degrees
 * - Moves to 90 degrees
 * - Moves to 180 degrees
 * - Then continuously waves back and forth like a hand wave
 */

#include <Servo.h>

// Pin definition
#define SERVO_PIN 5    // Servo connected to Grove port D5

// Servo object
Servo myServo;

// Wave settings
int waveSpeed = 15;    // Delay between movements (ms) - lower = faster
int waveMin = 45;      // Minimum angle for wave
int waveMax = 135;     // Maximum angle for wave

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Servo Handwave");
  Serial.println("==============");

  // Attach servo to pin
  myServo.attach(SERVO_PIN);

  // Initial sequence: 0 -> 90 -> 180
  Serial.println("Starting initial sequence...");

  // Start at 0 degrees
  Serial.println("Moving to 0 degrees");
  myServo.write(0);
  delay(1000);

  // Move to 90 degrees
  Serial.println("Moving to 90 degrees");
  myServo.write(90);
  delay(1000);

  // Move to 180 degrees
  Serial.println("Moving to 180 degrees");
  myServo.write(180);
  delay(1000);

  Serial.println("Starting handwave loop!");
}

void loop() {
  // Wave from max to min (like waving hand down)
  for (int angle = waveMax; angle >= waveMin; angle--) {
    myServo.write(angle);
    delay(waveSpeed);
  }

  // Small pause at the end of wave
  delay(100);

  // Wave from min to max (like waving hand up)
  for (int angle = waveMin; angle <= waveMax; angle++) {
    myServo.write(angle);
    delay(waveSpeed);
  }

  // Small pause at the end of wave
  delay(100);
}
