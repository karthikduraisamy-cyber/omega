# Line Tracking Fix - Changes Summary

## Date: February 12, 2026

## Files Modified

### 1. ApplicationFunctionSet_xxx0.cpp
**Location**: `/ELEGOO Smart Robot Car Kit V4.0 2024.01.30/02 Manual & Main Code & APP/02 Main Program (Arduino UNO)/TB6612 & MPU6050/SmartRobotCarV4.0_V1_20230201/`

#### Changes Made:

**A. Enabled Debug Output (Line ~10)**
```cpp
// Added:
#define _Test_print 1  // Enable debug printing to see sensor values
```
- Allows you to see real-time sensor readings in Serial Monitor
- Critical for calibration and troubleshooting

**B. Updated ApplicationFunctionSet_Tracking() Function (Line ~411)**

**Before:**
- Used cached sensor values from `TrackingData_L/M/R`
- Limited debug output
- Commented out sensor reading code

**After:**
- Reads fresh sensor values directly: `AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L/M/R()`
- Enhanced debug output showing all critical values
- Prints every 500ms: sensor readings, ground status, and detection range

**Key improvements:**
1. **Fresh sensor reads** - More responsive to line changes
2. **Comprehensive debug** - Easy to see what's happening
3. **Better error messages** - Shows detection range for calibration

## What Was Wrong

### Primary Issues:
1. **Stale sensor data**: Using cached values instead of reading directly
2. **No visibility**: Debug output was disabled, making troubleshooting impossible
3. **Potential calibration mismatch**: Detection thresholds might not match your specific setup

### Root Cause:
The tracking function relied on `ApplicationFunctionSet_SensorDataUpdate()` to read sensors, but this only updates every 10ms and might not be synchronized with the tracking logic. The working demo code reads sensors directly in the tracking function for immediate response.

## How It Works Now

### 1. Tracking Mode Activation
- Press **Button 1** on the car to activate line tracking mode
- Serial Monitor will start showing sensor readings

### 2. Line Detection Logic
```
Middle sensor on line (250-850) → Move Forward
Right sensor on line (250-850)  → Turn Right
Left sensor on line (250-850)   → Turn Left
All sensors off line            → Blind scan (search pattern)
All sensors > 950               → STOP (car lifted/safety)
```

### 3. Blind Scan Pattern
When the car loses the line, it performs a search pattern:
- 0-200ms: Turn right
- 200-1600ms: Turn left
- 1600-2000ms: Turn right again
- 3000-3500ms: Give up and stop

### 4. Safety Feature
If all three sensors read > 950, the car assumes it's been lifted off the ground and stops immediately.

## Next Steps for Testing

### Step 1: Upload Code
1. Open the Arduino IDE
2. Load: `SmartRobotCarV4.0_V1_20230201.ino`
3. Verify/Compile (Ctrl+R / Cmd+R)
4. Upload to Arduino (Ctrl+U / Cmd+U)

### Step 2: Monitor and Calibrate
1. Open Serial Monitor (Ctrl+Shift+M / Cmd+Shift+M)
2. Set baud rate to **9600**
3. Press **Button 1** to activate tracking mode
4. Watch the output:
```
ITR20001: L=245 M=520 R=312 | Ground=YES | Range=[250-850]
```

### Step 3: Test on Track
1. Place car on your black line track
2. Check if middle sensor reads 250-850 when on line
3. Check if sensors read >850 when on white surface
4. If values don't match, calibrate (see below)

### Step 4: Calibrate if Needed
If sensor readings don't match the expected range:

**Open**: `ApplicationFunctionSet_xxx0.h`

**Find** (around line 70-72):
```cpp
uint8_t TrackingDetection_S = 250;   // Lower threshold
uint16_t TrackingDetection_E = 850;  // Upper threshold
uint16_t TrackingDetection_V = 950;  // Ground detection
```

**Adjust** based on your readings:
- If black line reads 300-700: Set `S=280, E=720`
- If white surface reads 800-1000: Set `V=850`

### Step 5: Disable Debug (Optional)
Once working, disable debug for better performance:

In `ApplicationFunctionSet_xxx0.cpp`, change:
```cpp
#define _Test_print 1  // Enable
```
to:
```cpp
#define _Test_print 0  // Disable
```

## Expected Behavior

### ✅ Working Correctly:
- Car follows black line smoothly
- Corrects when drifting left or right
- Searches for line when lost (oscillates)
- Stops after ~3 seconds if can't find line
- Serial Monitor shows changing sensor values

### ❌ Still Having Issues:
- Car doesn't move at all → Check ground detection threshold
- Car ignores line → Check sensor calibration values
- Erratic movement → Check sensor connections
- No Serial output → Check Serial Monitor baud rate (9600)

## Troubleshooting Quick Reference

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| No Serial output | Wrong baud rate | Set to 9600 |
| Car doesn't move | Ground detection triggered | Lower `TrackingDetection_V` |
| Ignores black line | Values out of range | Adjust `S` and `E` thresholds |
| Stops on white | White reads as "lifted" | Increase `TrackingDetection_V` |
| All sensors read 0 | Not initialized | Check ITR20001 connections |
| Values don't change | Sensor malfunction | Check sensor LEDs are ON |

## Documentation Created

1. **LINE_TRACKING_FIX_GUIDE.md** - Complete troubleshooting guide
2. **SENSOR_CALIBRATION_REFERENCE.md** - Quick calibration reference
3. **CHANGES_SUMMARY.md** - This file

## Technical Details

### Sensor Hardware: ITR20001
- 3 infrared sensors (Left, Middle, Right)
- Returns analog values (0-1023)
- Lower values = darker surface (but depends on sensor variant)
- Default configuration: 250-850 = black line detection

### Detection Algorithm:
```cpp
inRange(value, TrackingDetection_S, TrackingDetection_E)
// Returns true if: TrackingDetection_S ≤ value ≤ TrackingDetection_E
```

### Motor Response Speed:
- Speed setting: 100 (range 0-255)
- In TraceBased_mode: No MPU6050 drift compensation
- Direct motor control for faster response to line changes

## Support

If you continue to have issues after following this guide:

1. ✅ Verified code uploaded successfully
2. ✅ Serial Monitor shows sensor readings
3. ✅ Sensor values change when moved over black line
4. ✅ Thresholds adjusted based on readings
5. ✅ Hardware connections checked

Still not working? Possible hardware issues:
- ITR20001 module fault
- Loose connections
- Low battery voltage (< 7.0V)
- Track surface issues (reflective, colored, uneven)

## Rollback Instructions

If you need to restore original code:
1. Navigate to tutorial folder: `03 Tutorial & Code/03 SmartRobotCarV4.0_Tracking/TB6612/`
2. Use Demo2 as reference for working tracking code
3. Or restore from the original main program folder (make a backup first!)

---
**Note**: Keep this file for reference when troubleshooting line tracking issues.

