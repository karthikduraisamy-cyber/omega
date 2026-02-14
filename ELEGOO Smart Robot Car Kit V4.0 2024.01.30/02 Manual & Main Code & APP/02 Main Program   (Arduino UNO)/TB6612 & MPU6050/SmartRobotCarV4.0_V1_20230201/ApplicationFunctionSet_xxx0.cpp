/*
 * @Description: Smart Robot Car V4.0 - Memory Optimized
 */
#include <avr/wdt.h>
#include <string.h>
#include <stdlib.h>
#include "ApplicationFunctionSet_xxx0.h"
#include "DeviceDriverSet_xxx0.h"
#include "MPU6050_getdata.h"

// Minimal JSON parser - saves ~2KB over ArduinoJson
#define _Test_print 1  // Enable debug printing to see sensor values
static int getJsonInt(const char* json, const char* key) {
  char search[8];
  snprintf(search, sizeof(search), "\"%s\":", key);
  const char* p = strstr(json, search);
  if (!p) return 0;
  p += strlen(search);
  while (*p == ' ') p++;
  return atoi(p);
}
static void getJsonStr(const char* json, const char* key, char* out, uint8_t len) {
  char search[8];
  snprintf(search, sizeof(search), "\"%s\":\"", key);
  const char* p = strstr(json, search);
  if (!p) { out[0] = 0; return; }
  p += strlen(search);
  uint8_t i = 0;
  while (*p && *p != '"' && i < len-1) out[i++] = *p++;
  out[i] = 0;
}

ApplicationFunctionSet Application_FunctionSet;

/*Hardware device object list*/
MPU6050_getdata AppMPU6050getdata;
DeviceDriverSet_RBGLED AppRBG_LED;
DeviceDriverSet_Key AppKey;
DeviceDriverSet_ITR20001 AppITR20001;
DeviceDriverSet_Voltage AppVoltage;
DeviceDriverSet_Motor AppMotor;
DeviceDriverSet_ULTRASONIC AppULTRASONIC;
DeviceDriverSet_Servo AppServo;
DeviceDriverSet_IRrecv AppIRrecv;
DeviceDriverSet_Buzzer AppBuzzer;
DeviceDriverSet_LCD AppLCD;

#define inRange(x, s, e) ((s) <= (x) && (x) <= (e))
static void delay_xxx(uint16_t _ms) { wdt_reset(); while(_ms--) delay(1); }

/*Movement Direction Control List*/
enum SmartRobotCarMotionControl
{
  Forward,       //(1)
  Backward,      //(2)
  Left,          //(3)
  Right,         //(4)
  LeftForward,   //(5)
  LeftBackward,  //(6)
  RightForward,  //(7)
  RightBackward, //(8)
  stop_it,       //(9)
  StrafeLeft,    //(10) Mecanum strafe
  StrafeRight,   //(11) Mecanum strafe
  DriftLeft,     //(12) Drift turn
  DriftRight     //(13) Drift turn
};               //direction方向:（1）、（2）、 （3）、（4）、（5）、（6）

/*Mode Control List*/
enum SmartRobotCarFunctionalModel
{
  Standby_mode,           /*Standby Mode*/
  TraceBased_mode,        /*Line Tracking Mode*/
  ObstacleAvoidance_mode, /*Obstacle Avoidance Mode*/
  Follow_mode,            /*Following Mode*/
  Rocker_mode,            /*Rocker Control Mode*/
  Omni_mode,              /*Omni-Directional Mode (Mecanum)*/
  Drift_mode,             /*Drift Mode*/
  CMD_inspect,
  CMD_Programming_mode,                   /*Programming Mode*/
  CMD_ClearAllFunctions_Standby_mode,     /*Clear All Functions And Enter Standby Mode*/
  CMD_ClearAllFunctions_Programming_mode, /*Clear All Functions And Enter Programming Mode*/
  CMD_MotorControl,                       /*Motor Control Mode*/
  CMD_CarControl_TimeLimit,               /*Car Movement Direction Control With Time Limit*/
  CMD_CarControl_NoTimeLimit,             /*Car Movement Direction Control Without Time Limit*/
  CMD_MotorControl_Speed,                 /*Motor Speed Control*/
  CMD_ServoControl,                       /*Servo Motor Control*/
  CMD_LightingControl_TimeLimit,          /*RGB Lighting Control With Time Limit*/
  CMD_LightingControl_NoTimeLimit,        /*RGB Lighting Control Without Time Limit*/

};

/*Application Management list*/
struct Application_xxx
{
  SmartRobotCarMotionControl Motion_Control;
  SmartRobotCarFunctionalModel Functional_Mode;
  unsigned long CMD_CarControl_Millis;
  unsigned long CMD_LightingControl_Millis;
};
Application_xxx Application_SmartRobotCarxxx0;

bool ApplicationFunctionSet_SmartRobotCarLeaveTheGround(void);
void ApplicationFunctionSet_SmartRobotCarLinearMotionControl(SmartRobotCarMotionControl direction, uint8_t directionRecord, uint8_t speed, uint8_t Kp, uint8_t UpperLimit);
void ApplicationFunctionSet_SmartRobotCarMotionControl(SmartRobotCarMotionControl direction, uint8_t is_speed);

void ApplicationFunctionSet::ApplicationFunctionSet_Init(void)
{
  bool res_error = true;
  Serial.begin(9600);
  AppVoltage.DeviceDriverSet_Voltage_Init();
  AppMotor.DeviceDriverSet_Motor_Init();
  AppServo.DeviceDriverSet_Servo_Init(90);
  AppKey.DeviceDriverSet_Key_Init();
  AppRBG_LED.DeviceDriverSet_RBGLED_Init(20);
  AppIRrecv.DeviceDriverSet_IRrecv_Init();
  AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Init();
  AppITR20001.DeviceDriverSet_ITR20001_Init();
  AppBuzzer.DeviceDriverSet_Buzzer_Init();
  AppLCD.DeviceDriverSet_LCD_Init();
  res_error = AppMPU6050getdata.MPU6050_dveInit();
  AppMPU6050getdata.MPU6050_calibration();

  // while (Serial.read() >= 0)
  // {
  //   /*Clear serial port buffer...*/
  // }
  Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
}

/*ITR20001 Check if the car leaves the ground*/
static bool ApplicationFunctionSet_SmartRobotCarLeaveTheGround(void)
{
  if (AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R() > Application_FunctionSet.TrackingDetection_V &&
      AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M() > Application_FunctionSet.TrackingDetection_V &&
      AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L() > Application_FunctionSet.TrackingDetection_V)
  {
    Application_FunctionSet.Car_LeaveTheGround = false;
    return false;
  }
  else
  {
    Application_FunctionSet.Car_LeaveTheGround = true;
    return true;
  }
}
/*
  Straight line movement control：For dual-drive motors, due to frequent motor coefficient deviations and many external interference factors, 
  it is difficult for the car to achieve relative Straight line movement. For this reason, the feedback of the yaw control loop is added.
  direction：only forward/backward
  directionRecord：Used to update the direction and position data (Yaw value) when entering the function for the first time.
  speed：the speed range is 0~255
  Kp：Position error proportional constant（The feedback of improving location resuming status，will be modified according to different mode），improve damping control.
  UpperLimit：Maximum output upper limit control
*/
static void ApplicationFunctionSet_SmartRobotCarLinearMotionControl(SmartRobotCarMotionControl direction, uint8_t directionRecord, uint8_t speed, uint8_t Kp, uint8_t UpperLimit)
{
  static float Yaw; //Yaw
  static float yaw_So = 0;
  static uint8_t en = 110;
  static unsigned long is_time;
  if (en != directionRecord || millis() - is_time > 10)
  {
    AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_void, /*speed_A*/ 0,
                                           /*direction_B*/ direction_void, /*speed_B*/ 0, /*controlED*/ control_enable); //Motor control
    AppMPU6050getdata.MPU6050_dveGetEulerAngles(&Yaw);
    is_time = millis();
  }
  //if (en != directionRecord)
  if (en != directionRecord || Application_FunctionSet.Car_LeaveTheGround == false)
  {
    en = directionRecord;
    yaw_So = Yaw;
  }
  //Add proportional constant Kp to increase rebound effect
  int R = (Yaw - yaw_So) * Kp + speed;
  if (R > UpperLimit)
  {
    R = UpperLimit;
  }
  else if (R < 10)
  {
    R = 10;
  }
  int L = (yaw_So - Yaw) * Kp + speed;
  if (L > UpperLimit)
  {
    L = UpperLimit;
  }
  else if (L < 10)
  {
    L = 10;
  }
  if (direction == Forward) //Forward
  {
    AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_just, /*speed_A*/ R,
                                           /*direction_B*/ direction_just, /*speed_B*/ L, /*controlED*/ control_enable);
  }
  else if (direction == Backward) //Backward
  {
    AppMotor.DeviceDriverSet_Motor_control(/*direction_A*/ direction_back, /*speed_A*/ L,
                                           /*direction_B*/ direction_back, /*speed_B*/ R, /*controlED*/ control_enable);
  }
}
/*
  Movement Direction Control: Optimized version
*/
static void ApplicationFunctionSet_SmartRobotCarMotionControl(SmartRobotCarMotionControl direction, uint8_t speed)
{
  static uint8_t directionRecord = 0;
  uint8_t Kp = 10, UpperLimit = 255;

  // Set parameters based on mode
  if (Application_SmartRobotCarxxx0.Functional_Mode != Rocker_mode &&
      Application_SmartRobotCarxxx0.Functional_Mode != TraceBased_mode) {
    Kp = 2; UpperLimit = 180;
  }

  switch (direction)
  {
  case Forward:
    if (Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode)
      AppMotor.DeviceDriverSet_Motor_control(direction_just, speed, direction_just, speed, control_enable);
    else {
      ApplicationFunctionSet_SmartRobotCarLinearMotionControl(Forward, directionRecord, speed, Kp, UpperLimit);
      directionRecord = 1;
    }
    break;
  case Backward:
    if (Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode)
      AppMotor.DeviceDriverSet_Motor_control(direction_back, speed, direction_back, speed, control_enable);
    else {
      ApplicationFunctionSet_SmartRobotCarLinearMotionControl(Backward, directionRecord, speed, Kp, UpperLimit);
      directionRecord = 2;
    }
    break;
  case Left:
    directionRecord = 3;
    AppMotor.DeviceDriverSet_Motor_control(direction_just, speed, direction_back, speed, control_enable);
    break;
  case Right:
    directionRecord = 4;
    AppMotor.DeviceDriverSet_Motor_control(direction_back, speed, direction_just, speed, control_enable);
    break;
  case LeftForward:
    directionRecord = 5;
    AppMotor.DeviceDriverSet_Motor_control(direction_just, speed, direction_just, speed / 2, control_enable);
    break;
  case LeftBackward:
    directionRecord = 6;
    AppMotor.DeviceDriverSet_Motor_control(direction_back, speed, direction_back, speed / 2, control_enable);
    break;
  case RightForward:
    directionRecord = 7;
    AppMotor.DeviceDriverSet_Motor_control(direction_just, speed / 2, direction_just, speed, control_enable);
    break;
  case RightBackward:
    directionRecord = 8;
    AppMotor.DeviceDriverSet_Motor_control(direction_back, speed / 2, direction_back, speed, control_enable);
    break;
  case stop_it:
    directionRecord = 9;
    AppMotor.DeviceDriverSet_Motor_control(direction_void, 0, direction_void, 0, control_enable);
    break;
  case StrafeLeft:  // Mecanum strafe left: alternating motor pulses
    directionRecord = 10;
    // For 2-motor setup, simulate strafe with rapid alternating
    AppMotor.DeviceDriverSet_Motor_control(direction_back, speed, direction_just, speed, control_enable);
    break;
  case StrafeRight:  // Mecanum strafe right: alternating motor pulses
    directionRecord = 11;
    AppMotor.DeviceDriverSet_Motor_control(direction_just, speed, direction_back, speed, control_enable);
    break;
  case DriftLeft:  // Drift left: forward with sharp left bias
    directionRecord = 12;
    AppMotor.DeviceDriverSet_Motor_control(direction_just, speed, direction_back, speed / 3, control_enable);
    break;
  case DriftRight:  // Drift right: forward with sharp right bias
    directionRecord = 13;
    AppMotor.DeviceDriverSet_Motor_control(direction_back, speed / 3, direction_just, speed, control_enable);
    break;
  default:
    directionRecord = 14;
    break;
  }
}
/*
 Robot car update sensors' data:Partial update (selective update)
*/
void ApplicationFunctionSet::ApplicationFunctionSet_SensorDataUpdate(void)
{

  // AppMotor.DeviceDriverSet_Motor_Test();
  { /*Battery voltage status update*/
    static unsigned long VoltageData_time = 0;
    static int VoltageData_number = 1;
    if (millis() - VoltageData_time > 10) //read and update the data per 10ms
    {
      VoltageData_time = millis();
      VoltageData_V = AppVoltage.DeviceDriverSet_Voltage_getAnalogue();
      if (VoltageData_V < VoltageDetection)
      {
        VoltageData_number++;
        if (VoltageData_number == 500) //Continuity to judge the latest voltage value multiple 
        {
          VoltageDetectionStatus = true;
          VoltageData_number = 0;
        }
      }
      else
      {
        VoltageDetectionStatus = false;
      }
    }
  }

  // { /*value updation for the ultrasonic sensor：for the Obstacle Avoidance mode*/
  //   AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&UltrasoundData_cm /*out*/);
  //   UltrasoundDetectionStatus = function_xxx(UltrasoundData_cm, 0, ObstacleDetection);
  // }

  { /*value updation for the IR sensors on the line tracking module：for the line tracking mode*/
    TrackingData_R = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R();
    TrackingDetectionStatus_R = inRange(TrackingData_R, TrackingDetection_S, TrackingDetection_E);
    TrackingData_M = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M();
    TrackingDetectionStatus_M = inRange(TrackingData_M, TrackingDetection_S, TrackingDetection_E);
    TrackingData_L = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L();
    TrackingDetectionStatus_L = inRange(TrackingData_L, TrackingDetection_S, TrackingDetection_E);
    //ITR20001 Check if the car leaves the ground
    ApplicationFunctionSet_SmartRobotCarLeaveTheGround();
  }

  // acquire timestamp
  // static unsigned long Test_time;
  // if (millis() - Test_time > 200)
  // {
  //   Test_time = millis();
  //   //AppITR20001.DeviceDriverSet_ITR20001_Test();
  // }
}
/*
  Startup operation requirement：
*/
void ApplicationFunctionSet::ApplicationFunctionSet_Bootup(void)
{
  Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
}

static void CMD_Lighting(uint8_t is_LightingSequence, int8_t is_LightingColorValue_R, uint8_t is_LightingColorValue_G, uint8_t is_LightingColorValue_B)
{
  switch (is_LightingSequence)
  {
  case 0:
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(NUM_LEDS, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  case 1: /*Left*/
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(3, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  case 2: /*Forward*/
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(2, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  case 3: /*Right*/
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(1, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  case 4: /*Back*/
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(0, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  case 5: /*Middle*/
    AppRBG_LED.DeviceDriverSet_RBGLED_Color(4, is_LightingColorValue_R, is_LightingColorValue_G, is_LightingColorValue_B);
    break;
  default:
    break;
  }
}

/*RBG_LED set*/
void ApplicationFunctionSet::ApplicationFunctionSet_RGB(void)
{
  static unsigned long time = 0;
  if (millis() - time < 100) return;
  time = millis();

  FastLED.clear(true);
  if (VoltageDetectionStatus) {
    AppRBG_LED.leds[0] = ((millis() / 200) & 1) ? CRGB::Black : CRGB::Red;
  } else {
    switch (Application_SmartRobotCarxxx0.Functional_Mode)
    {
    case Standby_mode: AppRBG_LED.leds[0] = CRGB::Violet; break;
    case TraceBased_mode: AppRBG_LED.leds[0] = CRGB::Green; break;
    case ObstacleAvoidance_mode: AppRBG_LED.leds[0] = CRGB::Yellow; break;
    case Follow_mode: AppRBG_LED.leds[0] = CRGB::Blue; break;
    case Rocker_mode: AppRBG_LED.leds[0] = CRGB::Violet; break;
    case Omni_mode: AppRBG_LED.leds[0] = CRGB::Cyan; break;
    case Drift_mode: AppRBG_LED.leds[0] = CRGB::OrangeRed; break;
    default: break;
    }
  }
  FastLED.show();
}

/*Rocker / Omni / Drift control - direction input drives motion; LCD shows active mode.
  Rocker_mode: original behavior unchanged (Forward/Backward/Left/Right as-is).
  Omni_mode: Left/Right mapped to StrafeLeft/StrafeRight. Drift_mode: Left/Right to DriftLeft/DriftRight.*/
void ApplicationFunctionSet::ApplicationFunctionSet_Rocker(void)
{
  SmartRobotCarFunctionalModel m = Application_SmartRobotCarxxx0.Functional_Mode;
  if (m != Rocker_mode && m != Omni_mode && m != Drift_mode) return;

  SmartRobotCarMotionControl dir = Application_SmartRobotCarxxx0.Motion_Control;
  if (m == Omni_mode) {
    if (dir == Left) dir = StrafeLeft;
    else if (dir == Right) dir = StrafeRight;
  } else if (m == Drift_mode) {
    if (dir == Left) dir = DriftLeft;
    else if (dir == Right) dir = DriftRight;
  }
  /* Rocker_mode: dir unchanged (Forward/Backward/Left/Right) - same as before */
  ApplicationFunctionSet_SmartRobotCarMotionControl(dir, Rocker_CarSpeed);
}

/*Line tracking mode - Tracks black line (ITR20001: black = low ADC, white = high ADC)*/
void ApplicationFunctionSet::ApplicationFunctionSet_Tracking(void)
{
  static boolean first_is = true;
  static boolean timestamp = true;
  static unsigned long MotorRL_time = 0;
  if (Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode)
  {
    if (first_is == true) //Enter the mode for the first time, center servo
    {
      AppServo.DeviceDriverSet_Servo_control(90 /*Position_angle*/);
      first_is = false;
    }

    /* Stop when car is lifted (all sensors read very high) */
    if (Car_LeaveTheGround == false)
    {
      ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
      return;
    }

    // Read fresh sensor values (L/M/R = left, middle, right from car's perspective)
    int getAnaloguexxx_L = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L();
    int getAnaloguexxx_M = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M();
    int getAnaloguexxx_R = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R();

    // "On black" = value in [S, E] (standard: black gives low reading, so 0..E)
    boolean onBlack_L = inRange(getAnaloguexxx_L, TrackingDetection_S, TrackingDetection_E);
    boolean onBlack_M = inRange(getAnaloguexxx_M, TrackingDetection_S, TrackingDetection_E);
    boolean onBlack_R = inRange(getAnaloguexxx_R, TrackingDetection_S, TrackingDetection_E);

#if _Test_print
    static unsigned long print_time = 0;
    if (millis() - print_time > 500)
    {
      print_time = millis();
      Serial.print("L=");
      Serial.print(getAnaloguexxx_L);
      Serial.print(" M=");
      Serial.print(getAnaloguexxx_M);
      Serial.print(" R=");
      Serial.print(getAnaloguexxx_R);
      Serial.print(" [");
      Serial.print(TrackingDetection_S);
      Serial.print("-");
      Serial.print(TrackingDetection_E);
      Serial.print("] ");
      Serial.print(onBlack_L ? "L" : "-");
      Serial.print(onBlack_M ? "M" : "-");
      Serial.println(onBlack_R ? "R" : "-");
    }
#endif

    /* Priority: middle on line -> forward; else one side on line -> steer toward line; else blind search */
    if (onBlack_M)
    {
      ApplicationFunctionSet_SmartRobotCarMotionControl(Forward, 100);
      timestamp = true;
    }
    else if (onBlack_R && !onBlack_L)
    {
      ApplicationFunctionSet_SmartRobotCarMotionControl(Right, 100);
      timestamp = true;
    }
    else if (onBlack_L && !onBlack_R)
    {
      ApplicationFunctionSet_SmartRobotCarMotionControl(Left, 100);
      timestamp = true;
    }
    else if (onBlack_L && onBlack_R)
    {
      /* Both sides on black: line under car, go forward */
      ApplicationFunctionSet_SmartRobotCarMotionControl(Forward, 100);
      timestamp = true;
    }
    else
    {
      /* No sensor on black - lost line, execute blind search */
      if (timestamp == true)
      {
        timestamp = false;
        MotorRL_time = millis();
      }
      unsigned long elapsed = millis() - MotorRL_time;
      if (elapsed < 300)
        ApplicationFunctionSet_SmartRobotCarMotionControl(Right, 80);
      else if (elapsed < 900)
        ApplicationFunctionSet_SmartRobotCarMotionControl(Left, 80);
      else if (elapsed < 1200)
        ApplicationFunctionSet_SmartRobotCarMotionControl(Right, 80);
      else
        MotorRL_time = millis();
    }
  }
  else
  {
    first_is = true;
    timestamp = true;
    MotorRL_time = 0;
  }
}

/*
  Obstacle Avoidance Mode - Improved Navigation

  Features:
  - Full sector scanning before decision making
  - Weighted path selection based on obstacle distances
  - Left drift compensation (~5 degrees)
  - Smart reverse when needed before turning

  Memory optimized for Arduino UNO (32KB limit)
*/
#define OBSTACLE_DIST 30       // Detection threshold (cm)
#define OBSTACLE_CRITICAL 15   // Critical proximity (cm)
#define OBSTACLE_FAR 50        // Far detection for early avoidance

// Motor drift compensation - adjust to correct left pull
// Positive = compensate right, Negative = compensate left
#define MOTOR_DRIFT_COMP 5     // 5% speed boost to right motor to counter left drift

// Navigation state
static struct {
  uint8_t consecutiveObs;  // Consecutive obstacle count
  int8_t lastTurnDir;      // Last turn direction (-1=left, 1=right)
  int8_t headingBias;      // Accumulated heading bias from drift compensation
} g_nav = {0, 0, 0};

// Scan angles: 7 sectors from 15° to 165° in 25° increments
#define NUM_SECTORS 7
static const uint8_t g_scanAngles[NUM_SECTORS] = {15, 40, 65, 90, 115, 140, 165};

// Forward movement with drift compensation
static void moveForwardCompensated(uint8_t speed) {
  // Apply drift compensation: boost right motor slightly to counter left pull
  uint8_t rightBoost = (speed * MOTOR_DRIFT_COMP) / 100;
  uint8_t leftSpeed = speed;
  uint8_t rightSpeed = speed + rightBoost;

  // Ensure we don't exceed max speed
  if (rightSpeed > 255) rightSpeed = 255;

  AppMotor.DeviceDriverSet_Motor_control(direction_just, rightSpeed,
                                         direction_just, leftSpeed, control_enable);
}

// Perform full sector scan and collect obstacle data
static void scanAllSectors(uint16_t* sectorDist) {
  for (uint8_t i = 0; i < NUM_SECTORS; i++) {
    AppServo.DeviceDriverSet_Servo_control(g_scanAngles[i]);
    AppBuzzer.DeviceDriverSet_Buzzer_Beep(1800, 20);  // Soft click for each angle
    delay_xxx(65);  // Wait for servo to stabilize
    AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&sectorDist[i]);
    // Handle invalid readings (0 means no echo = far/clear)
    if (sectorDist[i] == 0) sectorDist[i] = 200;  // Treat as clear path
  }
  // Return to center
  AppServo.DeviceDriverSet_Servo_control(90);
  delay_xxx(50);
}

// Find best sector: highest distance with preference for forward direction
static uint8_t findBestSector(uint16_t* sectorDist, bool* needReverse) {
  int16_t bestScore = -32000;
  uint8_t bestSector = 3;  // Default: center (90°)
  uint8_t blockedCount = 0;

  *needReverse = false;

  for (uint8_t i = 0; i < NUM_SECTORS; i++) {
    // Base score from distance (larger = better)
    int16_t score = (int16_t)sectorDist[i] * 3;

    // Bonus for forward-facing sectors (less turning needed)
    // Center sector (index 3) gets max bonus
    int8_t forwardBonus = 40 - abs((int8_t)i - 3) * 10;
    score += forwardBonus;

    // Penalty for recent turn direction (avoid oscillation)
    int8_t sectorDir = (i > 3) ? 1 : ((i < 3) ? -1 : 0);
    if (g_nav.lastTurnDir != 0 && sectorDir == g_nav.lastTurnDir) {
      score -= 15;  // Small penalty for same direction
    }

    // Count blocked sectors
    if (sectorDist[i] < OBSTACLE_DIST) blockedCount++;

    if (score > bestScore && sectorDist[i] >= OBSTACLE_CRITICAL) {
      bestScore = score;
      bestSector = i;
    }
  }

  // Need reverse if most sectors are blocked or best path is still close
  if (blockedCount >= 5 || sectorDist[bestSector] < OBSTACLE_DIST) {
    *needReverse = true;
  }

  return bestSector;
}

// Calculate turn parameters from sector
static void getTurnParams(uint8_t sector, int8_t* turnAngle, int8_t* turnDir) {
  // Convert sector to angle offset from center
  // Sector 0 = 15° (far right), Sector 3 = 90° (center), Sector 6 = 165° (far left)
  int16_t targetAngle = g_scanAngles[sector];
  *turnAngle = (int8_t)(90 - targetAngle);  // Positive = turn right, negative = turn left
  *turnDir = (*turnAngle > 0) ? 1 : ((*turnAngle < 0) ? -1 : 0);
}

void ApplicationFunctionSet::ApplicationFunctionSet_Obstacle(void)
{
  static boolean first_is = true;
  static uint16_t sectorDist[NUM_SECTORS];  // Reusable scan buffer

  if (Application_SmartRobotCarxxx0.Functional_Mode != ObstacleAvoidance_mode) {
    first_is = true;
    g_nav.consecutiveObs = 0;
    return;
  }

  uint16_t frontDist;

  // Initialize
  if (first_is) {
    AppServo.DeviceDriverSet_Servo_control(90);  // Center servo
    first_is = false;
    g_nav.consecutiveObs = 0;
    g_nav.lastTurnDir = 0;
    g_nav.headingBias = 0;
    delay_xxx(100);
  }

  // Get front distance (servo at center)
  AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&frontDist);
  if (frontDist == 0) frontDist = 200;  // Treat 0 as clear

  // === CRITICAL PROXIMITY: Emergency stop and reverse ===
  if (frontDist < OBSTACLE_CRITICAL) {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    AppBuzzer.DeviceDriverSet_Buzzer_Beep(4000, 200);
    delay_xxx(50);
    AppBuzzer.DeviceDriverSet_Buzzer_Beep(4000, 200);
    AppLCD.DeviceDriverSet_LCD_ObstacleAlert(true);

    // Emergency reverse
    ApplicationFunctionSet_SmartRobotCarMotionControl(Backward, 180);
    delay_xxx(450);
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);

    g_nav.consecutiveObs++;
    goto do_full_scan;
  }

  // === OBSTACLE DETECTED: Stop and analyze ===
  if (frontDist < OBSTACLE_DIST) {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    g_nav.consecutiveObs++;

    // Alert
    AppBuzzer.DeviceDriverSet_Buzzer_Beep(4000, 150);
    delay_xxx(60);
    AppBuzzer.DeviceDriverSet_Buzzer_Beep(4000, 150);
    AppLCD.DeviceDriverSet_LCD_ObstacleAlert(true);

do_full_scan:
    // === FULL SECTOR SCAN: Collect all obstacle data before deciding ===
    scanAllSectors(sectorDist);

    // Find best path and determine if reverse is needed
    bool needReverse = false;
    uint8_t bestSector = findBestSector(sectorDist, &needReverse);

    // Get turn parameters
    int8_t turnAngle, turnDir;
    getTurnParams(bestSector, &turnAngle, &turnDir);

    // === REVERSE IF NEEDED ===
    if (needReverse || g_nav.consecutiveObs >= 2) {
      // Triple beep for reverse
      AppBuzzer.DeviceDriverSet_Buzzer_Beep(3500, 60);
      delay_xxx(40);
      AppBuzzer.DeviceDriverSet_Buzzer_Beep(3500, 60);
      delay_xxx(40);
      AppBuzzer.DeviceDriverSet_Buzzer_Beep(3500, 60);

      // Reverse further if consecutive obstacles
      uint16_t reverseTime = (g_nav.consecutiveObs >= 2) ? 600 : 350;
      ApplicationFunctionSet_SmartRobotCarMotionControl(Backward, 180);
      delay_xxx(reverseTime);
      ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
      delay_xxx(80);
    }

    // === EXECUTE TURN toward best sector ===
    if (abs(turnAngle) > 10) {  // Only turn if significant angle
      // Calculate turn time proportional to angle
      // Approximate: 100ms per 30 degrees
      uint16_t turnTime = (uint16_t)abs(turnAngle) * 100 / 30;
      turnTime = constrain(turnTime, 80, 400);

      // Turn speed: higher for larger angles
      uint8_t turnSpeed = 110 + abs(turnAngle) / 3;
      turnSpeed = constrain(turnSpeed, 110, 160);

      // Execute turn
      if (turnAngle > 0) {  // Turn right
        ApplicationFunctionSet_SmartRobotCarMotionControl(Right, turnSpeed);
        g_nav.lastTurnDir = 1;
      } else {  // Turn left
        ApplicationFunctionSet_SmartRobotCarMotionControl(Left, turnSpeed);
        g_nav.lastTurnDir = -1;
      }
      delay_xxx(turnTime);
      ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);

      // Small beep to indicate turn complete
      AppBuzzer.DeviceDriverSet_Buzzer_Beep(2200, 40);
    }

    // === VERIFY PATH IS CLEAR ===
    AppServo.DeviceDriverSet_Servo_control(90);  // Ensure centered
    delay_xxx(60);

    uint16_t verifyDist;
    AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&verifyDist);
    if (verifyDist == 0) verifyDist = 200;

    // If still blocked, try incremental turns
    if (verifyDist < OBSTACLE_DIST) {
      int8_t searchDir = (g_nav.lastTurnDir != 0) ? g_nav.lastTurnDir : 1;
      bool foundClear = false;

      // Try 4 increments of ~30° in preferred direction
      for (uint8_t attempt = 0; attempt < 4 && !foundClear; attempt++) {
        AppBuzzer.DeviceDriverSet_Buzzer_Beep(1500, 30);
        ApplicationFunctionSet_SmartRobotCarMotionControl(searchDir > 0 ? Right : Left, 120);
        delay_xxx(100);
        ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
        delay_xxx(70);

        AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&verifyDist);
        if (verifyDist == 0 || verifyDist >= OBSTACLE_DIST) {
          foundClear = true;
          AppBuzzer.DeviceDriverSet_Buzzer_Beep(2500, 60);
        }
      }

      // If still blocked, try opposite direction
      if (!foundClear) {
        searchDir = -searchDir;
        for (uint8_t attempt = 0; attempt < 6 && !foundClear; attempt++) {
          AppBuzzer.DeviceDriverSet_Buzzer_Beep(1500, 30);
          ApplicationFunctionSet_SmartRobotCarMotionControl(searchDir > 0 ? Right : Left, 120);
          delay_xxx(100);
          ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
          delay_xxx(70);

          AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&verifyDist);
          if (verifyDist == 0 || verifyDist >= OBSTACLE_DIST) {
            foundClear = true;
            AppBuzzer.DeviceDriverSet_Buzzer_Beep(2500, 60);
          }
        }
      }
    }

    // Reset consecutive count after avoidance
    if (verifyDist >= OBSTACLE_DIST || verifyDist == 0) {
      g_nav.consecutiveObs = 0;
    }

    // Brief forward movement with drift compensation
    AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&verifyDist);
    if (verifyDist == 0 || verifyDist >= OBSTACLE_DIST) {
      moveForwardCompensated(140);
      delay_xxx(250);
    }

    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    AppLCD.DeviceDriverSet_LCD_ObstacleAlert(false);
  }
  else {
    // === CLEAR PATH: Move forward with drift compensation ===
    g_nav.consecutiveObs = 0;
    AppLCD.DeviceDriverSet_LCD_ObstacleAlert(false);

    // Speed based on distance - slower as obstacles get closer
    uint8_t speed;
    if (frontDist > OBSTACLE_FAR) {
      speed = 170;  // Full speed for clear path
    } else {
      // Proportional slowdown as we approach obstacle threshold
      speed = 100 + ((frontDist - OBSTACLE_DIST) * 70 / (OBSTACLE_FAR - OBSTACLE_DIST));
    }

    // Move forward with drift compensation
    moveForwardCompensated(speed);
  }
}

/*
  Following mode：
*/
void ApplicationFunctionSet::ApplicationFunctionSet_Follow(void)
{
  static uint16_t ULTRASONIC_Get = 0;
  static uint8_t Position_Servo = 1;
  static unsigned long time_Servo = 0;
  static const uint8_t servoAngles[] = {80, 20, 80, 150};

  if (Application_SmartRobotCarxxx0.Functional_Mode != Follow_mode) {
    ULTRASONIC_Get = 0;
    return;
  }

  if (!Car_LeaveTheGround) {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    return;
  }

  AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&ULTRASONIC_Get);

  if (ULTRASONIC_Get > 20) { // No obstacle within 20cm - scan
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    if (millis() - time_Servo > 1000) {
      time_Servo = millis();
      Position_Servo++;
      if (Position_Servo > 4) Position_Servo = 1;
    }
    AppServo.DeviceDriverSet_Servo_control(servoAngles[Position_Servo - 1]);
  } else { // Target detected - follow
    if (Position_Servo == 1 || Position_Servo == 3)
      ApplicationFunctionSet_SmartRobotCarMotionControl(Forward, 100);
    else if (Position_Servo == 2)
      ApplicationFunctionSet_SmartRobotCarMotionControl(Right, 150);
    else
      ApplicationFunctionSet_SmartRobotCarMotionControl(Left, 150);
  }
}

/*Servo motor control*/
void ApplicationFunctionSet::ApplicationFunctionSet_Servo(uint8_t Set_Servo)
{
  static int8_t z_angle = 9, y_angle = 9;

  if (Set_Servo == 1) y_angle--;
  else if (Set_Servo == 2) y_angle++;
  else if (Set_Servo == 3) z_angle++;
  else if (Set_Servo == 4) z_angle--;
  else if (Set_Servo == 5) { y_angle = z_angle = 9; }

  y_angle = constrain(y_angle, 3, 11);
  z_angle = constrain(z_angle, 1, 17);

  if (Set_Servo <= 2) AppServo.DeviceDriverSet_Servo_controls(2, y_angle);
  else if (Set_Servo <= 4) AppServo.DeviceDriverSet_Servo_controls(1, z_angle);
  else if (Set_Servo == 5) {
    AppServo.DeviceDriverSet_Servo_controls(2, 9);
    AppServo.DeviceDriverSet_Servo_controls(1, 9);
  }
}
/*Standby mode*/
void ApplicationFunctionSet::ApplicationFunctionSet_Standby(void)
{
  static bool is_ED = true;
  static uint8_t cout = 0;
  if (Application_SmartRobotCarxxx0.Functional_Mode == Standby_mode)
  {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    if (true == is_ED) //Used to zero yaw raw data(Make sure the car is placed on a stationary surface!)
    {
      static unsigned long timestamp; //acquire timestamp
      if (millis() - timestamp > 20)
      {
        timestamp = millis();
        if (ApplicationFunctionSet_SmartRobotCarLeaveTheGround() /* condition */)
        {
          cout += 1;
        }
        else
        {
          cout = 0;
        }
        if (cout > 10)
        {
          is_ED = false;
          AppMPU6050getdata.MPU6050_calibration();
        }
      }
    }
  }
}

/*LCD Display Update - all user modes (0-6) displayed and updated when mode changes*/
void ApplicationFunctionSet::ApplicationFunctionSet_LCD(void)
{
  uint8_t lcdMode = 0;
  switch (Application_SmartRobotCarxxx0.Functional_Mode) {
    case Standby_mode:           lcdMode = 0; break;
    case TraceBased_mode:        lcdMode = 1; break;
    case ObstacleAvoidance_mode: lcdMode = 2; break;
    case Follow_mode:            lcdMode = 3; break;
    case Rocker_mode:            lcdMode = 4; break;
    case Omni_mode:              lcdMode = 5; break;
    case Drift_mode:             lcdMode = 6; break;
    default:                     lcdMode = 0; break;  // CMD_* and others show Standby
  }
  AppLCD.DeviceDriverSet_LCD_SetMode(lcdMode);
  AppLCD.DeviceDriverSet_LCD_Update();
}

/*
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Begin:CMD
 * Graphical programming and command control module
 $ Elegoo & SmartRobot & 2020-06
*/

void ApplicationFunctionSet::CMD_inspect_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode == CMD_inspect)
  {
    delay(100);
  }
}

/*N1: Motor Control - Optimized */
void ApplicationFunctionSet::CMD_MotorControl_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode != CMD_MotorControl) return;
  if (!CMD_is_MotorDirection) { ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0); return; }
  uint8_t d = (CMD_is_MotorDirection == 1) ? direction_just : direction_back;
  AppMotor.DeviceDriverSet_Motor_control(
    (CMD_is_MotorSelection != 2) ? d : direction_void, (CMD_is_MotorSelection != 2) ? CMD_is_MotorSpeed : 0,
    (CMD_is_MotorSelection != 1) ? d : direction_void, (CMD_is_MotorSelection != 1) ? CMD_is_MotorSpeed : 0, control_enable);
}

static void CMD_CarControl(uint8_t is_CarDirection, uint8_t is_CarSpeed)
{
  switch (is_CarDirection)
  {
  case 1: ApplicationFunctionSet_SmartRobotCarMotionControl(Left, is_CarSpeed); break;
  case 2: ApplicationFunctionSet_SmartRobotCarMotionControl(Right, is_CarSpeed); break;
  case 3: ApplicationFunctionSet_SmartRobotCarMotionControl(Forward, is_CarSpeed); break;
  case 4: ApplicationFunctionSet_SmartRobotCarMotionControl(Backward, is_CarSpeed); break;
  default: break;
  }
}

/*N2: Car Control with Time Limit - Optimized */
void ApplicationFunctionSet::CMD_CarControlTimeLimit_xxx0(void)
{
  static boolean CarControl = false;
  if (Application_SmartRobotCarxxx0.Functional_Mode != CMD_CarControl_TimeLimit) {
    if (CarControl) { CarControl = false; Application_SmartRobotCarxxx0.CMD_CarControl_Millis = 0; }
    return;
  }
  CarControl = true;
  if (CMD_is_CarTimer && (millis() - Application_SmartRobotCarxxx0.CMD_CarControl_Millis) > CMD_is_CarTimer) {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_Programming_mode;
  } else {
    CMD_CarControl(CMD_is_CarDirection, CMD_is_CarSpeed);
  }
}
/*N3: Car Control No Time Limit - Optimized */
void ApplicationFunctionSet::CMD_CarControlNoTimeLimit_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode == CMD_CarControl_NoTimeLimit)
    CMD_CarControl(CMD_is_CarDirection, CMD_is_CarSpeed);
}

/*N4: Motor Control Speed - Optimized */
void ApplicationFunctionSet::CMD_MotorControlSpeed_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode != CMD_MotorControl_Speed) return;
  if (!CMD_is_MotorSpeed_L && !CMD_is_MotorSpeed_R)
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
  else
    AppMotor.DeviceDriverSet_Motor_control(direction_just, CMD_is_MotorSpeed_L, direction_just, CMD_is_MotorSpeed_R, control_enable);
}

/*N5: Servo Control*/
void ApplicationFunctionSet::CMD_ServoControl_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode == CMD_ServoControl)
  {
    AppServo.DeviceDriverSet_Servo_controls(CMD_is_Servo, CMD_is_Servo_angle / 10);
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_Programming_mode;
  }
}

/*N7: Lighting Control with Time Limit*/
void ApplicationFunctionSet::CMD_LightingControlTimeLimit_xxx0(void)
{
  static boolean LightingControl = false;
  if (Application_SmartRobotCarxxx0.Functional_Mode != CMD_LightingControl_TimeLimit) {
    if (LightingControl) {
      LightingControl = false;
      Application_SmartRobotCarxxx0.CMD_LightingControl_Millis = 0;
    }
    return;
  }

  LightingControl = true;
  if (CMD_is_LightingTimer != 0 && (millis() - Application_SmartRobotCarxxx0.CMD_LightingControl_Millis) > CMD_is_LightingTimer) {
    FastLED.clear(true);
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_Programming_mode;
    return;
  }
  CMD_Lighting(CMD_is_LightingSequence, CMD_is_LightingColorValue_R, CMD_is_LightingColorValue_G, CMD_is_LightingColorValue_B);
}
/*N8: Lighting Control No Time Limit - Optimized */
void ApplicationFunctionSet::CMD_LightingControlNoTimeLimit_xxx0(void)
{
  if (Application_SmartRobotCarxxx0.Functional_Mode == CMD_LightingControl_NoTimeLimit)
    CMD_Lighting(CMD_is_LightingSequence, CMD_is_LightingColorValue_R, CMD_is_LightingColorValue_G, CMD_is_LightingColorValue_B);
}

/*
  N100/N110:command - Clear all functions
*/
void ApplicationFunctionSet::CMD_ClearAllFunctions_xxx0(void)
{
  SmartRobotCarFunctionalModel mode = Application_SmartRobotCarxxx0.Functional_Mode;
  if (mode == CMD_ClearAllFunctions_Standby_mode || mode == CMD_ClearAllFunctions_Programming_mode)
  {
    ApplicationFunctionSet_SmartRobotCarMotionControl(stop_it, 0);
    FastLED.clear(true);
    AppRBG_LED.DeviceDriverSet_RBGLED_xxx(0, NUM_LEDS, CRGB::Black);
    Application_SmartRobotCarxxx0.Motion_Control = stop_it;
    Application_SmartRobotCarxxx0.Functional_Mode = (mode == CMD_ClearAllFunctions_Standby_mode) ? Standby_mode : CMD_Programming_mode;
  }
}

/*
  N21:command - Ultrasonic module status
*/
void ApplicationFunctionSet::CMD_UltrasoundModuleStatus_xxx0(uint8_t is_get)
{
  AppULTRASONIC.DeviceDriverSet_ULTRASONIC_Get(&UltrasoundData_cm);
  UltrasoundDetectionStatus = inRange(UltrasoundData_cm, 0, ObstacleDetection);
}

/*
  N22:command - Tracking module status
*/
void ApplicationFunctionSet::CMD_TraceModuleStatus_xxx0(uint8_t is_get)
{
  Application_SmartRobotCarxxx0.Functional_Mode = CMD_Programming_mode;
}

/* 
 * End:CMD
 * Graphical programming and command control module
 $ Elegoo & SmartRobot & 2020-06
 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*Key command*/
void ApplicationFunctionSet::ApplicationFunctionSet_KeyCommand(void)
{
  uint8_t get_keyValue;
  static uint8_t temp_keyValue = keyValue_Max;
  AppKey.DeviceDriverSet_key_Get(&get_keyValue);

  if (temp_keyValue != get_keyValue)
  {
    temp_keyValue = get_keyValue;//Serial.println(get_keyValue);
    switch (get_keyValue)
    {
    case /* constant-expression */ 1:
      /* code */
      Application_SmartRobotCarxxx0.Functional_Mode = TraceBased_mode;
      break;
    case /* constant-expression */ 2:
      /* code */
      Application_SmartRobotCarxxx0.Functional_Mode = ObstacleAvoidance_mode;
      break;
    case /* constant-expression */ 3:
      /* code */
      Application_SmartRobotCarxxx0.Functional_Mode = Follow_mode;
      break;
    case /* constant-expression */ 4:
      /* code */
      Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
      break;
    default:

      break;
    }
  }
}
/*Infrared remote control*/
void ApplicationFunctionSet::ApplicationFunctionSet_IRrecv(void)
{
  uint8_t IRrecv_button = 0;
  static bool IRrecv_en = false;
  if (AppIRrecv.DeviceDriverSet_IRrecv_Get(&IRrecv_button))
    IRrecv_en = true;

  if (!IRrecv_en) return;

  // Handle direction controls (buttons 1-4) - keep current mode if already Rocker/Omni/Drift
  if (IRrecv_button >= 1 && IRrecv_button <= 4) {
    static const SmartRobotCarMotionControl dirs[] = {Forward, Backward, Left, Right};
    Application_SmartRobotCarxxx0.Motion_Control = dirs[IRrecv_button - 1];
    if (Application_SmartRobotCarxxx0.Functional_Mode != Rocker_mode &&
        Application_SmartRobotCarxxx0.Functional_Mode != Omni_mode &&
        Application_SmartRobotCarxxx0.Functional_Mode != Drift_mode)
      Application_SmartRobotCarxxx0.Functional_Mode = Rocker_mode;
    if (millis() - AppIRrecv.IR_PreMillis > 300) {
      IRrecv_en = false;
      Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
      AppIRrecv.IR_PreMillis = millis();
    }
    return;
  }

  // Handle mode buttons (5-8)
  if (IRrecv_button == 5) Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
  else if (IRrecv_button == 6) Application_SmartRobotCarxxx0.Functional_Mode = TraceBased_mode;
  else if (IRrecv_button == 7) Application_SmartRobotCarxxx0.Functional_Mode = ObstacleAvoidance_mode;
  else if (IRrecv_button == 8) Application_SmartRobotCarxxx0.Functional_Mode = Follow_mode;
  // Handle tracking sensitivity (9-11)
  else if (IRrecv_button == 9 && Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode && TrackingDetection_S < 600)
    TrackingDetection_S += 10;
  else if (IRrecv_button == 10 && Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode)
    TrackingDetection_S = 250;
  else if (IRrecv_button == 11 && Application_SmartRobotCarxxx0.Functional_Mode == TraceBased_mode && TrackingDetection_S > 30)
    TrackingDetection_S -= 10;
  // Handle speed (12-14)
  else if (IRrecv_button == 12 && Rocker_CarSpeed < 255) Rocker_CarSpeed += 5;
  else if (IRrecv_button == 13) Rocker_CarSpeed = 250;
  else if (IRrecv_button == 14 && Rocker_CarSpeed > 50) Rocker_CarSpeed -= 5;
  else Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;

  IRrecv_en = false;
  AppIRrecv.IR_PreMillis = millis();
}
/*Data analysis on serial port*/
void ApplicationFunctionSet::ApplicationFunctionSet_SerialPortDataAnalysis(void)
{
  static char buf[48];
  static uint8_t idx = 0;

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '{') idx = 0;
    if (idx < sizeof(buf)-1) buf[idx++] = c;
    if (c == '}') break;
  }
  if (idx == 0 || buf[idx-1] != '}') return;
  buf[idx] = 0;
  idx = 0;

  int control_mode_N = getJsonInt(buf, "N");
  getJsonStr(buf, "H", CommandSerialNumber, sizeof(CommandSerialNumber));

  switch (control_mode_N)
  {
  case 1: // Motor control
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_MotorControl;
    CMD_is_MotorSelection = getJsonInt(buf, "D1");
    CMD_is_MotorSpeed = getJsonInt(buf, "D2");
    CMD_is_MotorDirection = getJsonInt(buf, "D3");
    break;
  case 2: // Car control with time limit
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_CarControl_TimeLimit;
    CMD_is_CarDirection = getJsonInt(buf, "D1");
    CMD_is_CarSpeed = getJsonInt(buf, "D2");
    CMD_is_CarTimer = getJsonInt(buf, "T");
    Application_SmartRobotCarxxx0.CMD_CarControl_Millis = millis();
    break;
  case 3: // Car control no time limit
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_CarControl_NoTimeLimit;
    CMD_is_CarDirection = getJsonInt(buf, "D1");
    CMD_is_CarSpeed = getJsonInt(buf, "D2");
    break;
  case 4: // Motor speed control
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_MotorControl_Speed;
    CMD_is_MotorSpeed_L = getJsonInt(buf, "D1");
    CMD_is_MotorSpeed_R = getJsonInt(buf, "D2");
    break;
  case 5: // Servo control
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_ServoControl;
    CMD_is_Servo = getJsonInt(buf, "D1");
    CMD_is_Servo_angle = getJsonInt(buf, "D2");
    break;
  case 7: // Lighting with time limit
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_LightingControl_TimeLimit;
    CMD_is_LightingSequence = getJsonInt(buf, "D1");
    CMD_is_LightingColorValue_R = getJsonInt(buf, "D2");
    CMD_is_LightingColorValue_G = getJsonInt(buf, "D3");
    CMD_is_LightingColorValue_B = getJsonInt(buf, "D4");
    CMD_is_LightingTimer = getJsonInt(buf, "T");
    Application_SmartRobotCarxxx0.CMD_LightingControl_Millis = millis();
    break;
  case 8: // Lighting no time limit
    Application_SmartRobotCarxxx0.Functional_Mode = CMD_LightingControl_NoTimeLimit;
    CMD_is_LightingSequence = getJsonInt(buf, "D1");
    CMD_is_LightingColorValue_R = getJsonInt(buf, "D2");
    CMD_is_LightingColorValue_G = getJsonInt(buf, "D3");
    CMD_is_LightingColorValue_B = getJsonInt(buf, "D4");
    break;
  case 21: CMD_UltrasoundModuleStatus_xxx0(getJsonInt(buf, "D1")); break;
  case 22: CMD_TraceModuleStatus_xxx0(getJsonInt(buf, "D1")); break;
  case 23: break;
  case 100: Application_SmartRobotCarxxx0.Functional_Mode = CMD_ClearAllFunctions_Standby_mode; break;
  case 110: Application_SmartRobotCarxxx0.Functional_Mode = CMD_ClearAllFunctions_Programming_mode; break;
  case 101: { // Mode switch: 0=Standby, 1=Track, 2=Obstacle, 3=Follow, 4=Rocker, 5=Omni, 6=Drift
    uint8_t mode = getJsonInt(buf, "D1");
    switch (mode) {
      case 0: Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode; break;
      case 1: Application_SmartRobotCarxxx0.Functional_Mode = TraceBased_mode; break;
      case 2: Application_SmartRobotCarxxx0.Functional_Mode = ObstacleAvoidance_mode; break;
      case 3: Application_SmartRobotCarxxx0.Functional_Mode = Follow_mode; break;
      case 4: Application_SmartRobotCarxxx0.Functional_Mode = Rocker_mode; break;
      case 5: Application_SmartRobotCarxxx0.Functional_Mode = Omni_mode; break;
      case 6: Application_SmartRobotCarxxx0.Functional_Mode = Drift_mode; break;
      default: break;
    }
    break;
  }
  case 105: { // Brightness
    uint8_t d1 = getJsonInt(buf, "D1");
    if (d1 == 1 && CMD_is_FastLED_setBrightness < 250) CMD_is_FastLED_setBrightness += 5;
    else if (d1 == 2 && CMD_is_FastLED_setBrightness > 0) CMD_is_FastLED_setBrightness -= 5;
    FastLED.setBrightness(CMD_is_FastLED_setBrightness);
    break;
  }
  case 106: { // Servo direct
    uint8_t servoCmd = getJsonInt(buf, "D1");
    if (servoCmd >= 1 && servoCmd <= 5) ApplicationFunctionSet_Servo(servoCmd);
    break;
  }
  case 102: { // Rocker control
    Application_SmartRobotCarxxx0.Functional_Mode = Rocker_mode;
    Rocker_temp = getJsonInt(buf, "D1");
    Rocker_CarSpeed = getJsonInt(buf, "D2");
    static const SmartRobotCarMotionControl rockerDirs[] = {
      Forward, Backward, Left, Right, LeftForward, LeftBackward, RightForward, RightBackward, stop_it
    };
    if (Rocker_temp >= 1 && Rocker_temp <= 9) {
      Application_SmartRobotCarxxx0.Motion_Control = rockerDirs[Rocker_temp - 1];
      if (Rocker_temp == 9) Application_SmartRobotCarxxx0.Functional_Mode = Standby_mode;
    } else {
      Application_SmartRobotCarxxx0.Motion_Control = stop_it;
    }
    break;
  }
  default: break;
  }
}
