/*
 * @Description: Smart Robot Car V4.0 - Device Drivers (Optimized)
 */
#include "DeviceDriverSet_xxx0.h"
#include <avr/wdt.h>

static void delay_xxx(uint16_t _ms) { wdt_reset(); while(_ms--) delay(1); }
/*RBG LED*/
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}
void DeviceDriverSet_RBGLED::DeviceDriverSet_RBGLED_xxx(uint16_t Duration, uint8_t Traversal_Number, CRGB colour)
{
  if (NUM_LEDS < Traversal_Number)
  {
    Traversal_Number = NUM_LEDS;
  }
  for (int Number = 0; Number < Traversal_Number; Number++)
  {
    leds[Number] = colour;
    FastLED.show();
    delay_xxx(Duration);
  }
}
void DeviceDriverSet_RBGLED::DeviceDriverSet_RBGLED_Init(uint8_t set_Brightness)
{
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(set_Brightness);
}
#if _Test_DeviceDriverSet
void DeviceDriverSet_RBGLED::DeviceDriverSet_RBGLED_Test(void)
{
  leds[0] = CRGB::White;
  FastLED.show();
  delay_xxx(50);
  leds[1] = CRGB::Red;
  FastLED.show();
  delay_xxx(50);
  DeviceDriverSet_RBGLED_xxx(50 /*Duration*/, 5 /*Traversal_Number*/, CRGB::Black);
}
#endif

void DeviceDriverSet_RBGLED::DeviceDriverSet_RBGLED_Color(uint8_t LED_s, uint8_t r, uint8_t g, uint8_t b)
{
  if (LED_s > NUM_LEDS)
    return;
  if (LED_s == NUM_LEDS)
  {
    FastLED.showColor(Color(r, g, b));
  }
  else
  {
    leds[LED_s] = Color(r, g, b);
  }
  FastLED.show();
}

/*Key*/
uint8_t DeviceDriverSet_Key::keyValue = 0;

static void attachPinChangeInterrupt_GetKeyValue(void)
{
  DeviceDriverSet_Key Key;
  static uint32_t keyValue_time = 0;
  static uint8_t keyValue_temp = 0;
  if ((millis() - keyValue_time) > 500)
  {
    keyValue_temp++;
    keyValue_time = millis();
    if (keyValue_temp > keyValue_Max)
    {
      keyValue_temp = 0;
    }
    Key.keyValue = keyValue_temp;
  }
}
void DeviceDriverSet_Key::DeviceDriverSet_Key_Init(void)
{
  pinMode(PIN_Key, INPUT_PULLUP);
  //attachPinChangeInterrupt(PIN_Key, attachPinChangeInterrupt_GetKeyValue, FALLING);
  attachInterrupt(0, attachPinChangeInterrupt_GetKeyValue, FALLING);
}

#if _Test_DeviceDriverSet
void DeviceDriverSet_Key::DeviceDriverSet_Key_Test(void)
{
  Serial.println(DeviceDriverSet_Key::keyValue);
}
#endif

void DeviceDriverSet_Key::DeviceDriverSet_key_Get(uint8_t *get_keyValue)
{
  *get_keyValue = keyValue;
}

/*ITR20001 Detection*/
bool DeviceDriverSet_ITR20001::DeviceDriverSet_ITR20001_Init(void)
{
  pinMode(PIN_ITR20001xxxL, INPUT);
  pinMode(PIN_ITR20001xxxM, INPUT);
  pinMode(PIN_ITR20001xxxR, INPUT);
  return false;
}
int DeviceDriverSet_ITR20001::DeviceDriverSet_ITR20001_getAnaloguexxx_L(void)
{
  return analogRead(PIN_ITR20001xxxL);
}
int DeviceDriverSet_ITR20001::DeviceDriverSet_ITR20001_getAnaloguexxx_M(void)
{
  return analogRead(PIN_ITR20001xxxM);
}
int DeviceDriverSet_ITR20001::DeviceDriverSet_ITR20001_getAnaloguexxx_R(void)
{
  return analogRead(PIN_ITR20001xxxR);
}
#if _Test_DeviceDriverSet
void DeviceDriverSet_ITR20001::DeviceDriverSet_ITR20001_Test(void)
{
  Serial.print("\tL=");
  Serial.print(analogRead(PIN_ITR20001xxxL));

  Serial.print("\tM=");
  Serial.print(analogRead(PIN_ITR20001xxxM));

  Serial.print("\tR=");
  Serial.println(analogRead(PIN_ITR20001xxxR));
}
#endif

/*Voltage Detection*/
void DeviceDriverSet_Voltage::DeviceDriverSet_Voltage_Init(void)
{
  pinMode(PIN_Voltage, INPUT);
  //analogReference(INTERNAL);
}
float DeviceDriverSet_Voltage::DeviceDriverSet_Voltage_getAnalogue(void)
{
  //float Voltage = ((analogRead(PIN_Voltage) * 5.00 / 1024) * 7.67); //7.66666=((10 + 1.50) / 1.50)
  float Voltage = (analogRead(PIN_Voltage) * 0.0375);
  Voltage = Voltage + (Voltage * 0.08); //Compensation 8%
  //return (analogRead(PIN_Voltage) * 5.00 / 1024) * ((10 + 1.50) / 1.50); //Read voltage value
  return Voltage;
}

#if _Test_DeviceDriverSet
void DeviceDriverSet_Voltage::DeviceDriverSet_Voltage_Test(void)
{
  //float Voltage = ((analogRead(PIN_Voltage) * 5.00 / 1024) * 7.67); //7.66666=((10 + 1.50) / 1.50)
  float Voltage = (analogRead(PIN_Voltage) * 0.0375); //7.66666=((10 + 1.50) / 1.50)
  Voltage = Voltage + (Voltage * 0.08);               //Compensation 8%
  //Serial.println(analogRead(PIN_Voltage) * 4.97 / 1024);
  Serial.println(Voltage);
}
#endif
/*Motor control*/
void DeviceDriverSet_Motor::DeviceDriverSet_Motor_Init(void)
{
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_STBY, OUTPUT);
}

#if _Test_DeviceDriverSet
void DeviceDriverSet_Motor::DeviceDriverSet_Motor_Test(void)
{
  //A...Right
  //B...Left
  digitalWrite(PIN_Motor_STBY, HIGH);

  digitalWrite(PIN_Motor_AIN_1, HIGH);
  analogWrite(PIN_Motor_PWMA, 100);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
  analogWrite(PIN_Motor_PWMB, 100);
  delay_xxx(1000);

  digitalWrite(PIN_Motor_STBY, LOW);
  delay_xxx(1000);
  digitalWrite(PIN_Motor_STBY, HIGH);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  analogWrite(PIN_Motor_PWMA, 100);
  digitalWrite(PIN_Motor_BIN_1, LOW);
  analogWrite(PIN_Motor_PWMB, 100);

  delay_xxx(1000);
}
#endif

/*
 Motor_control：AB / movement direction and speed - Optimized
*/
void DeviceDriverSet_Motor::DeviceDriverSet_Motor_control(boolean direction_A, uint8_t speed_A,
                                                          boolean direction_B, uint8_t speed_B,
                                                          boolean controlED)
{
  if (!controlED) { digitalWrite(PIN_Motor_STBY, LOW); return; }
  digitalWrite(PIN_Motor_STBY, HIGH);

  // Motor A (Right)
  if (direction_A == direction_void) { analogWrite(PIN_Motor_PWMA, 0); digitalWrite(PIN_Motor_STBY, LOW); }
  else { digitalWrite(PIN_Motor_AIN_1, direction_A); analogWrite(PIN_Motor_PWMA, speed_A); }

  // Motor B (Left)
  if (direction_B == direction_void) { analogWrite(PIN_Motor_PWMB, 0); digitalWrite(PIN_Motor_STBY, LOW); }
  else { digitalWrite(PIN_Motor_BIN_1, direction_B); analogWrite(PIN_Motor_PWMB, speed_B); }
}

/*ULTRASONIC*/
//#include <NewPing.h>
// NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
void DeviceDriverSet_ULTRASONIC::DeviceDriverSet_ULTRASONIC_Init(void)
{
  pinMode(ECHO_PIN, INPUT); //Ultrasonic module initialization
  pinMode(TRIG_PIN, OUTPUT);
}
void DeviceDriverSet_ULTRASONIC::DeviceDriverSet_ULTRASONIC_Get(uint16_t *ULTRASONIC_Get /*out*/)
{
  // Multi-sample with median filtering for accuracy
  uint16_t samples[3];

  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(15);  // Longer pulse for stronger signal
    digitalWrite(TRIG_PIN, LOW);
    samples[i] = (uint16_t)(pulseIn(ECHO_PIN, HIGH, 25000) / 58);
    delayMicroseconds(500);  // Brief delay between samples
  }

  // Simple median of 3: sort and take middle
  if (samples[0] > samples[1]) { uint16_t t = samples[0]; samples[0] = samples[1]; samples[1] = t; }
  if (samples[1] > samples[2]) { uint16_t t = samples[1]; samples[1] = samples[2]; samples[2] = t; }
  if (samples[0] > samples[1]) { uint16_t t = samples[0]; samples[0] = samples[1]; samples[1] = t; }

  *ULTRASONIC_Get = (samples[1] > 150) ? 150 : samples[1];
}

#if _Test_DeviceDriverSet
void DeviceDriverSet_ULTRASONIC::DeviceDriverSet_ULTRASONIC_Test(void)
{

  unsigned int tempda = 0;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);  // Longer settle time
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(15); // Increased pulse duration for stronger signal
  digitalWrite(TRIG_PIN, LOW);
  tempda = ((unsigned int)pulseIn(ECHO_PIN, HIGH, 30000) / 58);

  // if (tempda_x > 50)
  // {
  //   tempda_x = 50;
  // }

  // // return tempda;
  // return tempda_x;

  Serial.print("ULTRASONIC=");
  Serial.print(tempda); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
}

#endif

/*Servo*/

Servo myservo; // create servo object to control a servo
void DeviceDriverSet_Servo::DeviceDriverSet_Servo_Init(unsigned int Position_angle)
{
  // Initialize Z-axis servo (ultrasonic sensor) to face straight
  // Apply calibration offset for mechanical alignment
  int center_angle = 90 + SERVO_Z_OFFSET;
  center_angle = constrain(center_angle, 0, 180);

  myservo.attach(PIN_Servo_z, 500, 2400);
  myservo.write(center_angle);  // Corrected center position
  delay_xxx(500);     // Longer delay to ensure servo reaches position
  myservo.detach();

  // Initialize Y-axis servo
  myservo.attach(PIN_Servo_y, 500, 2400);
  myservo.write(Position_angle);
  delay_xxx(300);
  myservo.detach();
}
#if _Test_DeviceDriverSet
void DeviceDriverSet_Servo::DeviceDriverSet_Servo_Test(void)
{
  for (;;)
  {
    myservo.attach(PIN_Servo_z);
    myservo.write(180);
    delay_xxx(500);
    myservo.write(0);
    delay_xxx(500);
  }

  // for (uint8_t i = 0; i < 6; i++)
  // {
  //   myservo.write(30 * i);
  //   delay(500);
  // }
  // for (uint8_t i = 6; i > 0; i--)
  // {
  //   myservo.write(30 * i);
  //   delay(500);
  // }

  // myservo.attach(PIN_Servo_y);

  // for (uint8_t i = 0; i < 6; i++)
  // {
  //   myservo.write(30 * i);
  //   delay(500);
  // }
  // for (uint8_t i = 6; i > 0; i--)
  // {
  //   myservo.write(30 * i);
  //   delay(500);
  // }
}
#endif

/*0.17sec/60degree(4.8v) - Apply calibration offset for Z-axis servo*/
void DeviceDriverSet_Servo::DeviceDriverSet_Servo_control(unsigned int Position_angle)
{
  // Apply calibration offset and constrain to valid range
  int corrected_angle = (int)Position_angle + SERVO_Z_OFFSET;
  corrected_angle = constrain(corrected_angle, 0, 180);

  myservo.attach(PIN_Servo_z);
  myservo.write(corrected_angle);
  delay_xxx(350);
  myservo.detach();
}
//Servo motor control:Servo motor number and position angle - Optimized
void DeviceDriverSet_Servo::DeviceDriverSet_Servo_controls(uint8_t Servo, unsigned int Position_angle)
{
  uint8_t pin = (Servo == 2) ? PIN_Servo_y : PIN_Servo_z;
  uint8_t minA = (Servo == 2) ? 3 : 1;
  uint8_t maxA = (Servo == 2) ? 11 : 17;
  Position_angle = constrain(Position_angle, minA, maxA);
  myservo.attach(pin);
  myservo.write(10 * Position_angle);
  delay_xxx(350);
  myservo.detach();
}

/*IRrecv*/
IRrecv irrecv(RECV_PIN);
decode_results results;
void DeviceDriverSet_IRrecv::DeviceDriverSet_IRrecv_Init(void) { irrecv.enableIRIn(); }

/*Buzzer - Simple implementation without tone() to avoid Timer2 conflict with IRremote*/
void DeviceDriverSet_Buzzer::DeviceDriverSet_Buzzer_Init(void)
{
  pinMode(PIN_Buzzer, OUTPUT);
  digitalWrite(PIN_Buzzer, LOW);
}
void DeviceDriverSet_Buzzer::DeviceDriverSet_Buzzer_Beep(uint16_t freq, uint16_t duration)
{
  // Simple square wave generation without using tone() (avoids Timer2 conflict)
  uint16_t period = 1000000UL / freq;  // Period in microseconds
  uint16_t halfPeriod = period / 2;
  unsigned long endTime = millis() + duration;
  while (millis() < endTime) {
    digitalWrite(PIN_Buzzer, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(PIN_Buzzer, LOW);
    delayMicroseconds(halfPeriod);
  }
}
void DeviceDriverSet_Buzzer::DeviceDriverSet_Buzzer_Off(void)
{
  digitalWrite(PIN_Buzzer, LOW);
}

/*LCD Display - Grove RGB LCD with Project Omega display (memory optimized)*/
static rgb_lcd lcd;

// PROGMEM strings - Standby mode scrolling names
static const char PROGMEM lcd_scroll[] = " VIKRAM AYAAN SIDDHARTH ";
#define SCROLL_LEN 24

// Mode display strings in PROGMEM
static const char PROGMEM str_omega[] = "PROJECT OMEGA";
static const char PROGMEM str_track[] = "LANE TRACKING";
static const char PROGMEM str_obst[] = "OBSTACLE DETECT";
static const char PROGMEM str_follow[] = "FOLLOW MODE";
static const char PROGMEM str_rocker[] = "ROCKER";
static const char PROGMEM str_omni[] = "OMNI-DIRECTION";
static const char PROGMEM str_drift[] = "DRIFT MODE";
static const char PROGMEM str_alert1[] = "    OBSTACLE";
static const char PROGMEM str_alert2[] = "    DETECTED";

// Custom arrow characters
static const uint8_t PROGMEM arrow_l[] = {1,3,7,15,7,3,1,0};
static const uint8_t PROGMEM arrow_r[] = {16,24,28,30,28,24,16,0};

void DeviceDriverSet_LCD::DeviceDriverSet_LCD_Init(void)
{
  lcd.begin(16, 2);

  // Load custom characters from PROGMEM
  uint8_t buf[8];
  memcpy_P(buf, arrow_l, 8); lcd.createChar(1, buf);
  memcpy_P(buf, arrow_r, 8); lcd.createChar(2, buf);

  scrollPos = 0;
  currentMode = 0;
  lastScrollTime = 0;
  lastColorTime = 0;
  colorPhase = 0;
  obstacleAlert = false;

  // Display initial standby screen
  LCD_DisplayModeText(0);
  lcd.setRGB(255, 100, 0);  // Start with orange
}

void DeviceDriverSet_LCD::LCD_DisplayModeText(uint8_t mode)
{
  lcd.clear();
  lcd.setCursor(0, 0);

  char buf[17];
  switch (mode) {
    case 0:  // Standby - PROJECT OMEGA
      lcd.write(byte(1));  // Left arrow
      strcpy_P(buf, str_omega);
      lcd.print(buf);
      lcd.write(byte(2));  // Right arrow
      break;
    case 1:  // Line Tracking
      strcpy_P(buf, str_track);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Mode Active"));
      break;
    case 2:  // Obstacle Avoidance
      strcpy_P(buf, str_obst);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Mode Active"));
      break;
    case 3:  // Follow
      strcpy_P(buf, str_follow);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Mode Active"));
      break;
    case 4:  // Rocker
      strcpy_P(buf, str_rocker);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Mode Active"));
      break;
    case 5:  // Omni-Directional Mode
      strcpy_P(buf, str_omni);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Mecanum Wheels"));
      break;
    case 6:  // Drift Mode
      strcpy_P(buf, str_drift);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print(F("Slide & Drift!"));
      break;
    default:  // Unknown mode - ensure something is always shown
      lcd.print(F("MODE "));
      lcd.print(mode);
      lcd.setCursor(0, 1);
      lcd.print(F("Active"));
      break;
  }
}

void DeviceDriverSet_LCD::DeviceDriverSet_LCD_Update(void)
{
  // Don't update if showing obstacle alert
  if (obstacleAlert) return;

  unsigned long now = millis();

  // Only scroll in standby mode (mode 0)
  if (currentMode == 0 && now - lastScrollTime >= 300) {
    lastScrollTime = now;
    lcd.setCursor(0, 1);
    for (uint8_t i = 0; i < 16; i++) {
      uint8_t idx = (scrollPos + i) % SCROLL_LEN;
      lcd.print((char)pgm_read_byte(&lcd_scroll[idx]));
    }
    scrollPos = (scrollPos + 1) % SCROLL_LEN;
  }

  // Cycle colors only in standby mode
  if (currentMode == 0 && now - lastColorTime >= 150) {
    lastColorTime = now;
    colorPhase++;
    switch (colorPhase & 7) {
      case 0: lcd.setRGB(255, 0, 0); break;     // Red
      case 1: lcd.setRGB(255, 128, 0); break;   // Orange
      case 2: lcd.setRGB(255, 255, 0); break;   // Yellow
      case 3: lcd.setRGB(0, 255, 0); break;     // Green
      case 4: lcd.setRGB(0, 255, 255); break;   // Cyan
      case 5: lcd.setRGB(0, 0, 255); break;     // Blue
      case 6: lcd.setRGB(128, 0, 255); break;   // Purple
      case 7: lcd.setRGB(255, 0, 128); break;   // Pink
    }
  }
}

void DeviceDriverSet_LCD::DeviceDriverSet_LCD_SetMode(uint8_t mode)
{
  if (mode > 6) mode = 0;  // Clamp to valid display modes
  if (mode == currentMode && !obstacleAlert) return;
  currentMode = mode;
  obstacleAlert = false;
  LCD_DisplayModeText(mode);

  // Set color based on mode
  switch (mode) {
    case 1: lcd.setRGB(0, 255, 0); break;     // Line tracking - Green
    case 2: lcd.setRGB(255, 165, 0); break;   // Obstacle - Orange
    case 3: lcd.setRGB(0, 0, 255); break;     // Follow - Blue
    case 4: lcd.setRGB(255, 255, 0); break;   // Rocker - Yellow
    case 5: lcd.setRGB(0, 255, 255); break;   // Omni - Cyan
    case 6: lcd.setRGB(255, 69, 0); break;    // Drift - OrangeRed
    default: break;  // Standby uses color cycling
  }
}

void DeviceDriverSet_LCD::DeviceDriverSet_LCD_ObstacleAlert(bool detected)
{
  if (detected && !obstacleAlert) {
    obstacleAlert = true;
    lcd.clear();
    lcd.setRGB(255, 0, 0);  // RED background

    char buf[17];
    lcd.setCursor(0, 0);
    strcpy_P(buf, str_alert1);
    lcd.print(buf);
    lcd.setCursor(0, 1);
    strcpy_P(buf, str_alert2);
    lcd.print(buf);
  } else if (!detected && obstacleAlert) {
    obstacleAlert = false;
    LCD_DisplayModeText(currentMode);
    if (currentMode == 2) {
      lcd.setRGB(255, 165, 0);  // Back to orange for obstacle mode
    }
  }
}

// IR code lookup table - pairs of {codeA, codeB} for each button 1-14
static const uint32_t ir_codes[] PROGMEM = {
  aRECV_upper, bRECV_upper, aRECV_lower, bRECV_lower, aRECV_Left, bRECV_Left, aRECV_right, bRECV_right,
  aRECV_ok, bRECV_ok, aRECV_1, bRECV_1, aRECV_2, bRECV_2, aRECV_3, bRECV_3,
  aRECV_4, bRECV_4, aRECV_5, bRECV_5, aRECV_6, bRECV_6, aRECV_7, bRECV_7, aRECV_8, bRECV_8, aRECV_9, bRECV_9
};

bool DeviceDriverSet_IRrecv::DeviceDriverSet_IRrecv_Get(uint8_t *IRrecv_Get)
{
  if (!irrecv.decode(&results)) return false;
  IR_PreMillis = millis();
  uint32_t v = results.value;
  irrecv.resume();
  for (uint8_t i = 0; i < 14; i++) {
    if (v == pgm_read_dword(&ir_codes[i*2]) || v == pgm_read_dword(&ir_codes[i*2+1])) {
      *IRrecv_Get = i + 1;
      return true;
    }
  }
  return false;
}

#if _Test_DeviceDriverSet
void DeviceDriverSet_IRrecv::DeviceDriverSet_IRrecv_Test(void)
{
  if (irrecv.decode(&results))
  {
    Serial.print("IRrecv_Test:");
    Serial.println(results.value);
    irrecv.resume();
  }
}
#endif
