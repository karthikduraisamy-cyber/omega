# FIXED - Line Tracking Issues Resolved

## What Was Wrong

After comparing with the reference implementation, I found the tracking function had the correct logic but needed alignment with the working reference code.

## Changes Applied

### 1. Reverted to Cached Sensor Values
- **Issue**: Was trying to read sensors directly in tracking function
- **Fix**: Now uses `TrackingDetectionStatus_L/M/R` from `ApplicationFunctionSet_SensorDataUpdate()`
- **Why**: The sensor update runs first in the loop, providing consistent readings

### 2. Added Servo Initialization
- **Issue**: Servo wasn't centered when entering tracking mode
- **Fix**: Added `first_is` flag to center servo at 90° on first entry
- **Why**: Ensures consistent starting position like the reference code

### 3. Enhanced Debug Output
- Shows both raw values AND detection status
- Format: `ITR20001: L=xxx M=xxx R=xxx | DetL=x DetM=x DetR=x | Ground=YES | Range=[250-850]`
- Detection flags show 1 (detected black line) or 0 (not detected)

## How to Test

### Step 1: Upload Code
```bash
1. Open SmartRobotCarV4.0_V1_20230201.ino
2. Verify/Compile (✓ button)
3. Upload (→ button)
```

### Step 2: Open Serial Monitor
```bash
Tools → Serial Monitor
Set baud rate: 9600
```

### Step 3: Activate Tracking Mode
```bash
Press Button 1 on the car
You should see output like:
ITR20001: L=320 M=520 R=415 | DetL=1 DetM=1 DetR=1 | Ground=YES | Range=[250-850]
```

### Step 4: Test Sensor Response

**Place car on WHITE surface:**
```
Expected: All Det=0 (no detection)
L > 850, M > 850, R > 850
```

**Place MIDDLE sensor on black tape:**
```
Expected: DetM=1, DetL=0, DetR=0
M = 250-850, L > 850, R > 850
Car should: Move Forward
```

**Place RIGHT sensor on black tape:**
```
Expected: DetR=1, DetM=0, DetL=0
R = 250-850, M > 850, L > 850
Car should: Turn Right
```

**Place LEFT sensor on black tape:**
```
Expected: DetL=1, DetM=0, DetR=0
L = 250-850, M > 850, R > 850
Car should: Turn Left
```

**Lift car off ground:**
```
Expected: Ground=NO
All sensors > 950
Car should: STOP immediately
```

## Understanding the Detection Logic

### Detection Status Flags
The `DetL`, `DetM`, `DetR` flags show whether each sensor "sees" the black line:
- **1** = Black line detected (value is within 250-850 range)
- **0** = No line detected (value is outside range)

### Movement Logic
```cpp
if (DetM == 1)      → Move Forward (line is under middle sensor)
else if (DetR == 1) → Turn Right (line drifted to right sensor)
else if (DetL == 1) → Turn Left (line drifted to left sensor)
else                → Blind Scan (lost the line - search for it)
```

### Blind Scan Pattern (when all sensors = 0)
```
0-200ms:     Turn Right (quick check)
200-1600ms:  Turn Left (sweep across)
1600-2000ms: Turn Right (final check)
3000-3500ms: Give Up (stop if still not found)
```

## Troubleshooting Guide

### Problem: Car doesn't move at all

**Check Serial Monitor Output:**
```
If Ground=NO → Car thinks it's lifted
```

**Solution:**
```cpp
// In ApplicationFunctionSet_xxx0.h, increase threshold:
uint16_t TrackingDetection_V = 1000;  // Was 950
```

### Problem: Car ignores the black tape

**Check Serial Monitor Output:**
```
Place sensor on black tape
Look at the value (e.g., M=520)
```

**If value is OUTSIDE 250-850:**
```cpp
// In ApplicationFunctionSet_xxx0.h, adjust range:
// Example: if black reads 300-700
uint8_t TrackingDetection_S = 280;   // 20 below lowest
uint16_t TrackingDetection_E = 720;  // 20 above highest
```

### Problem: Detection flags always 0

**Check:**
1. ITR20001 module is powered (LEDs should be ON)
2. Sensors are 2-3mm from ground
3. Using BLACK tape on WHITE surface
4. Tape is matte (not glossy/shiny)

**Test in Serial Monitor:**
```
Place on white: All values should be > 850
Place on black: Values should be 250-850
If not, you need to calibrate thresholds
```

### Problem: Detection flags always 1

**This means range is too wide**
```cpp
// In ApplicationFunctionSet_xxx0.h:
// Narrow the detection range
uint8_t TrackingDetection_S = 350;   // Increase lower bound
uint16_t TrackingDetection_E = 750;  // Decrease upper bound
```

### Problem: Car turns wrong direction

**This is a wiring/sensor orientation issue:**
- Left and Right sensors may be swapped
- Check physical placement of ITR20001 module
- Should be: [L] [M] [R] from left to right

## Testing Checklist

- [ ] Serial Monitor shows sensor values updating every 500ms
- [ ] Servo centers to 90° when entering tracking mode
- [ ] All three detection flags (DetL, DetM, DetR) change when moved over black tape
- [ ] Middle sensor detection (DetM=1) makes car move forward
- [ ] Right sensor detection (DetR=1) makes car turn right
- [ ] Left sensor detection (DetL=1) makes car turn left
- [ ] All flags=0 triggers blind scan (oscillates left-right)
- [ ] Ground detection works (car stops when lifted)

## Expected Serial Output Examples

### Car on white surface, tracking active:
```
ITR20001: L=920 M=935 R=908 | DetL=0 DetM=0 DetR=0 | Ground=YES | Range=[250-850]
[Car performs blind scan - oscillates to find line]
```

### Car following black line (middle sensor):
```
ITR20001: L=892 M=520 R=877 | DetL=0 DetM=1 DetR=0 | Ground=YES | Range=[250-850]
[Car moves forward]
```

### Line drifting to right:
```
ITR20001: L=915 M=803 R=465 | DetL=0 DetM=0 DetR=1 | Ground=YES | Range=[250-850]
[Car turns right to get middle sensor back on line]
```

### Line drifting to left:
```
ITR20001: L=385 M=821 R=902 | DetL=1 DetM=0 DetR=0 | Ground=YES | Range=[250-850]
[Car turns left to get middle sensor back on line]
```

### Car lifted off ground:
```
ITR20001: L=1015 M=1022 R=1018 | DetL=0 DetM=0 DetR=0 | Ground=NO | Range=[250-850]
[Car stops immediately - safety feature]
```

## Hardware Requirements

### Track Requirements:
- **Line color**: Black
- **Background**: White
- **Line width**: 15-20mm (0.6-0.8 inches)
- **Surface**: Matte/non-reflective
- **Curves**: Gentle (not sharp 90° turns)
- **Material**: Black electrical tape or printed paper

### Car Requirements:
- **Battery**: > 7.0V (check voltage in Serial Monitor if available)
- **Sensors**: 2-3mm from ground
- **ITR20001**: LEDs should be ON when powered
- **Surface**: Flat and level

## Quick Calibration Table

Fill this out while testing:

| Test Condition | L Value | M Value | R Value | DetL | DetM | DetR | Ground | Action |
|----------------|---------|---------|---------|------|------|------|--------|--------|
| On white       | ____    | ____    | ____    | __   | __   | __   | ____   | Blind scan |
| Middle on black| ____    | ____    | ____    | __   | __   | __   | ____   | Forward |
| Right on black | ____    | ____    | ____    | __   | __   | __   | ____   | Turn Right |
| Left on black  | ____    | ____    | ____    | __   | __   | __   | ____   | Turn Left |
| Car lifted     | ____    | ____    | ____    | __   | __   | __   | ____   | STOP |

### Ideal Values:
- White: 850-1023
- Black: 250-850
- Lifted: 950-1023

If your values differ significantly, adjust thresholds in `ApplicationFunctionSet_xxx0.h`.

## After It Works

### Disable Debug Output (Optional)
Once tracking works perfectly:

**In `ApplicationFunctionSet_xxx0.cpp`:**
```cpp
// Change line ~12:
#define _Test_print 0  // Disable debug printing
```

This improves performance slightly by removing Serial output.

---

## Summary of Fix

The tracking function now matches the reference implementation:
1. ✅ Uses cached sensor values from update function
2. ✅ Centers servo on first entry to tracking mode
3. ✅ Shows comprehensive debug output with detection flags
4. ✅ Follows exact same logic as working reference code

The car should now properly follow black tape!

