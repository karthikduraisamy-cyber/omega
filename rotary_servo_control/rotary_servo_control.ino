/*
 * Rotary Angle Sensor Servo Control for Arduino Uno with Grove Base Shield
 *
 * Hardware:
 * - Arduino Uno
 * - Grove Base Shield
 * - Grove Rotary Angle Sensor (Potentiometer)
 * - Servo Motor
 *
 * Connections (Grove Base Shield):
 * - Rotary Angle Sensor: Connect to analog port A0
 * - Servo Motor: Connect to digital port D5
 *
 * Behavior:
 * - Rotary sensor position directly controls servo angle
 * - Turn knob right (clockwise) -> servo moves right (increases angle)
 * - Turn knob left (counter-clockwise) -> servo moves left (decreases angle)
 * - Angles are synchronized: sensor and servo stay in sync
 */

#include <Servo.h>

// Pin definitions
#define ROTARY_PIN A0    // Rotary angle sensor connected to analog port A0
#define SERVO_PIN 5      // Servo connected to digital port D5

// Rotary sensor parameters
// Grove rotary angle sensor has ~300 degree rotation
// ADC gives values 0-1023 for 0-5V input
#define ROTARY_MAX_ANGLE 300  // Maximum rotation angle of the sensor
#define ADC_MAX 1023          // Maximum ADC value (10-bit)

// Servo parameters
#define SERVO_MIN_ANGLE 0     // Minimum servo angle
#define SERVO_MAX_ANGLE 180   // Maximum servo angle

// Smoothing parameters
#define SMOOTHING_FACTOR 5    // Number of readings to average
#define UPDATE_DELAY 15       // Delay between updates (ms)

// Servo object
Servo myServo;

// Variables for smoothing
int readings[SMOOTHING_FACTOR];
int readIndex = 0;
int total = 0;
int average = 0;

// Previous angle to reduce servo jitter
int previousAngle = -1;
int angleThreshold = 2;  // Minimum change required to update servo

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Rotary Angle Sensor Servo Control");
  Serial.println("==================================");
  Serial.println("Turn the knob to control the servo!");
  Serial.println();

  // Initialize smoothing array
  for (int i = 0; i < SMOOTHING_FACTOR; i++) {
    readings[i] = 0;
  }

  // Attach servo to pin
  myServo.attach(SERVO_PIN);

  // Initialize servo to center position
  myServo.write(90);
  delay(500);

  Serial.println("Ready! Rotate the knob to move the servo.");
}

void loop() {
  // Read the rotary sensor value with smoothing
  int sensorValue = readSmoothedSensor();

  // Map sensor value (0-1023) to servo angle (0-180)
  // This ensures turning right increases the angle
  int servoAngle = map(sensorValue, 0, ADC_MAX, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

  // Constrain to valid servo range
  servoAngle = constrain(servoAngle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

  // Only update servo if angle changed significantly (reduces jitter)
  if (abs(servoAngle - previousAngle) >= angleThreshold || previousAngle == -1) {
    myServo.write(servoAngle);
    previousAngle = servoAngle;

    // Print debug info
    printDebugInfo(sensorValue, servoAngle);
  }

  delay(UPDATE_DELAY);
}

/*
 * Read sensor with smoothing (moving average)
 * Reduces noise and provides smoother servo movement
 */
int readSmoothedSensor() {
  // Subtract the last reading
  total = total - readings[readIndex];

  // Read from the sensor
  readings[readIndex] = analogRead(ROTARY_PIN);

  // Add the reading to the total
  total = total + readings[readIndex];

  // Advance to the next position in the array
  readIndex = (readIndex + 1) % SMOOTHING_FACTOR;

  // Calculate the average
  average = total / SMOOTHING_FACTOR;

  return average;
}

/*
 * Print debug information to serial monitor
 */
void printDebugInfo(int sensorValue, int servoAngle) {
  // Calculate approximate knob angle (0-300 degrees for Grove sensor)
  int knobAngle = map(sensorValue, 0, ADC_MAX, 0, ROTARY_MAX_ANGLE);

  Serial.print("Sensor: ");
  Serial.print(sensorValue);
  Serial.print(" | Knob Angle: ");
  Serial.print(knobAngle);
  Serial.print("° | Servo Angle: ");
  Serial.print(servoAngle);
  Serial.println("°");
}
