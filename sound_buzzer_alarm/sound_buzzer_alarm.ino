/*
 * Sound Sensor Buzzer Alarm for Arduino Uno with Grove Base Shield
 *
 * Hardware:
 * - Arduino Uno
 * - Grove Base Shield
 * - Grove Sound Sensor
 * - Grove Buzzer
 *
 * Connections (Grove Base Shield):
 * - Sound Sensor: Connect to Grove port A0 (analog)
 * - Buzzer: Connect to Grove port D3 (digital)
 *
 * Behavior:
 * - Continuously monitors sound level from the sound sensor
 * - When sound exceeds threshold, buzzer activates for exactly 5 seconds
 * - Buzzer stops after 5 seconds, then can be triggered again
 */

// Pin definitions for Grove Base Shield
#define SOUND_SENSOR_PIN A0  // Sound sensor connected to Grove port A0
#define BUZZER_PIN 3         // Buzzer connected to Grove port D3

// Threshold settings
// Note: Sound sensors output analog values (0-1023), not direct decibel readings
// Higher value = less sensitive (only triggers on louder sounds)
// Adjust this value based on your environment - increase if it triggers too easily
const int SOUND_THRESHOLD = 800;  // High threshold - only triggers on loud sounds (clapping, shouting)

// Variables
int soundValue = 0;           // Current sound sensor reading
bool buzzerActive = false;    // Track buzzer state
bool inCooldown = false;      // Cooldown period after buzzer stops

// Timing settings
unsigned long buzzerStartTime = 0;
unsigned long cooldownStartTime = 0;
const unsigned long BUZZER_DURATION = 5000;  // Buzzer stays on for 5 seconds (5000ms)
const unsigned long COOLDOWN_DURATION = 1000;  // 1 second cooldown after buzzer stops

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Sound Sensor Buzzer Alarm");
  Serial.println("=========================");

  // Set buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);

  // Ensure buzzer is off at startup
  digitalWrite(BUZZER_PIN, LOW);

  // Sound sensor pin is analog input (default)

  Serial.println("Ready! Monitoring sound levels...");
  Serial.print("Threshold set to: ");
  Serial.println(SOUND_THRESHOLD);
  Serial.println();
}

void loop() {
  // Get current time
  unsigned long currentTime = millis();

  // Handle cooldown period (after buzzer was turned off)
  if (inCooldown) {
    if (currentTime - cooldownStartTime >= COOLDOWN_DURATION) {
      inCooldown = false;
      Serial.println("Cooldown complete - Ready to detect sounds");
    }
    delay(10);
    return;  // Skip everything during cooldown
  }

  // If buzzer is active, only check if it's time to turn it off
  if (buzzerActive) {
    unsigned long elapsedTime = currentTime - buzzerStartTime;

    if (elapsedTime >= BUZZER_DURATION) {
      // Time's up - turn off buzzer
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      inCooldown = true;
      cooldownStartTime = currentTime;
      Serial.println("5 seconds elapsed - Buzzer OFF (entering cooldown)");
    } else {
      // Create warning tone pattern (beep on/off every 200ms)
      if ((elapsedTime / 200) % 2 == 0) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }
    }
    delay(10);
    return;  // Don't check sound while buzzer is on
  }

  // Buzzer is OFF and not in cooldown - read sound sensor
  soundValue = analogRead(SOUND_SENSOR_PIN);

  if (soundValue > SOUND_THRESHOLD) {
    // Sound is loud - activate buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = true;
    buzzerStartTime = millis();  // Use fresh timestamp

    // Print alert message
    Serial.print("ALERT! Loud sound detected: ");
    Serial.print(soundValue);
    Serial.println(" - Buzzer ON for 5 seconds");
  }

  // Print current sound level periodically for monitoring
  static unsigned long lastPrintTime = 0;
  if (currentTime - lastPrintTime > 500) {  // Print every 500ms
    Serial.print("Sound Level: ");
    Serial.print(soundValue);
    Serial.print(" | Threshold: ");
    Serial.print(SOUND_THRESHOLD);
    Serial.print(" | Buzzer: ");
    Serial.println(buzzerActive ? "ON" : "OFF");
    lastPrintTime = currentTime;
  }

  delay(10);
}
