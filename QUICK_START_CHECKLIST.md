# ✅ Line Tracking - Quick Start Checklist

## ⚡ IMMEDIATE ACTIONS

### 1. Upload Code to Arduino
- [ ] Open Arduino IDE
- [ ] File → Open → Navigate to:
      `TB6612 & MPU6050/SmartRobotCarV4.0_V1_20230201/SmartRobotCarV4.0_V1_20230201.ino`
- [ ] Click **Upload** button (→)
- [ ] Wait for "Done uploading" message

### 2. Open Serial Monitor
- [ ] Click Serial Monitor icon (magnifying glass) OR press Ctrl+Shift+M
- [ ] Set baud rate dropdown to: **9600**
- [ ] Keep this window open

### 3. Activate Tracking Mode
- [ ] Press **Button 1** on the robot car
- [ ] LED should change (mode indicator)
- [ ] Serial Monitor should start showing data

---

## 📊 WHAT YOU SHOULD SEE

### Normal Output (every 500ms):
```
ITR20001: L=245 M=520 R=312 | DetL=0 DetM=1 DetR=0 | Ground=YES | Range=[250-850]
```

### What Each Part Means:
```
L=245       → Left sensor value
M=520       → Middle sensor value (IN RANGE 250-850 = detecting black)
R=312       → Right sensor value
DetL=0      → Left not detecting (0=no, 1=yes)
DetM=1      → Middle IS detecting (black line detected!)
DetR=0      → Right not detecting
Ground=YES  → Car is on ground (not lifted)
Range=[250-850] → Current detection thresholds
```

---

## 🎯 SENSOR VALUE EXPECTATIONS

### Place Middle Sensor on BLACK LINE:
```
Expected: M = 250 to 850
Example:  M = 520 ← GOOD!
```

### Place All Sensors on WHITE Surface:
```
Expected: L, M, R > 850 (typically 900-1000)
Example:  L=945 M=920 R=938 ← GOOD!
```

### Lift Car Off Ground:
```
Expected: L, M, R > 950
Example:  L=1000 M=1010 R=1005 ← GOOD!
Car should STOP immediately
```

---

## ⚠️ COMMON PROBLEMS & INSTANT FIXES

### Problem 1: No Serial Output
```
Serial Monitor is blank
```
**Fix:**
- [ ] Check baud rate = 9600
- [ ] Check USB cable connected
- [ ] Press Button 1 to activate tracking mode
- [ ] Re-upload code if needed

---

### Problem 2: Car Doesn't Move
```
Serial shows: L=1000 M=1005 R=998 | Ground=NO
```
**Diagnosis:** Ground detection thinks car is lifted

**Fix:** Edit `ApplicationFunctionSet_xxx0.h` line 72:
```cpp
// OLD:
uint16_t TrackingDetection_V = 950;

// NEW:
uint16_t TrackingDetection_V = 1000;  // Or use (your_lowest_white_value - 50)
```
Then re-upload code.

---

### Problem 3: Car Ignores Black Line
```
Serial shows: M=320 (when on black) but DetM=0
Range=[250-850]
```
**Diagnosis:** Your black line reads outside the detection range

**Fix:** Edit `ApplicationFunctionSet_xxx0.h` lines 70-71:
```cpp
// If your black line reads 300-700:
uint8_t TrackingDetection_S = 280;   // 20 below lowest
uint16_t TrackingDetection_E = 720;  // 20 above highest

// If your black line reads 150-600:
uint8_t TrackingDetection_S = 130;
uint16_t TrackingDetection_E = 620;
```
Then re-upload code.

---

### Problem 4: Car Stops on White Surface
```
Serial shows: L=920 M=945 R=930 | Ground=NO
```
**Diagnosis:** White surface reads > 950, triggering ground detection

**Fix:** Edit `ApplicationFunctionSet_xxx0.h` line 72:
```cpp
uint16_t TrackingDetection_V = 960;  // Set higher than your white values
```
Then re-upload code.

---

## 🔧 CALIBRATION WORKFLOW

### Step 1: Collect Data
Place car in different positions and record Serial Monitor values:

| Position | L Value | M Value | R Value |
|----------|---------|---------|---------|
| White surface | _____ | _____ | _____ |
| Middle on black | _____ | _____ | _____ |
| Left on black | _____ | _____ | _____ |
| Right on black | _____ | _____ | _____ |
| Lifted in air | _____ | _____ | _____ |

### Step 2: Calculate Thresholds
```
MIN_BLACK = (lowest black reading from above table)
MAX_BLACK = (highest black reading from above table)
MIN_WHITE = (lowest white surface reading)

TrackingDetection_S = MIN_BLACK - 20
TrackingDetection_E = MAX_BLACK + 20
TrackingDetection_V = MIN_WHITE - 50
```

### Step 3: Apply Changes
1. Open `ApplicationFunctionSet_xxx0.h`
2. Find lines 70-72
3. Update the three values
4. Save file
5. Re-upload to Arduino

### Step 4: Verify
- [ ] Place on black → Serial shows Det=1
- [ ] Place on white → Serial shows Det=0
- [ ] Lift car → Car stops, Ground=NO

---

## 🏁 TRACK SETUP REQUIREMENTS

### ✅ GOOD Track Setup:
- **Surface**: White paper, poster board, or light-colored smooth floor
- **Line**: Matte black electrical tape (15-20mm wide)
- **Contrast**: Strong difference between black and white
- **Lighting**: Even, no harsh shadows
- **Curves**: Gentle (radius > 10cm)
- **Height**: Sensors 2-3mm from surface

### ❌ BAD Track Setup:
- Glossy or reflective tape
- Colored lines (red, blue, etc.)
- Textured or uneven surfaces
- Line too narrow (< 10mm)
- Sharp 90° turns
- Shadows or bright spot lights

---

## 📱 QUICK COMMANDS

### Activate Modes:
- **Button 1** = Line Tracking Mode
- **Button 2** = Obstacle Avoidance Mode
- **Button 3** = Following Mode
- **Button 4** = Standby Mode

### After Testing (Optional):
Disable debug output for better performance:

1. Open `ApplicationFunctionSet_xxx0.cpp`
2. Find line ~11:
   ```cpp
   #define _Test_print 1  // Enable
   ```
3. Change to:
   ```cpp
   #define _Test_print 0  // Disable
   ```
4. Save and re-upload

---

## 📚 DOCUMENTATION FILES

All in `/backup/` folder:
- **LINE_TRACKING_FIX_GUIDE.md** - Full troubleshooting guide
- **SENSOR_CALIBRATION_REFERENCE.md** - Calibration reference
- **CHANGES_SUMMARY.md** - Technical changes made
- **QUICK_START_CHECKLIST.md** - This file

---

## ✅ SUCCESS CRITERIA

Your line tracking is working when:
- [ ] Serial Monitor shows sensor values updating
- [ ] Values change when car moved over black line
- [ ] Middle on black → Car moves forward
- [ ] Right on black → Car turns right
- [ ] Left on black → Car turns left
- [ ] Car searches when line lost (blind scan)
- [ ] Car stops after timeout or when lifted

---

## 🆘 STILL STUCK?

If nothing works after following this checklist:

1. **Check Hardware:**
   - Look at ITR20001 module - are the LEDs ON?
   - Check all wire connections
   - Measure battery voltage (should be > 7.0V)

2. **Try Demo Code:**
   - Open: `03 Tutorial & Code/03 SmartRobotCarV4.0_Tracking/TB6612/Demo2/Demo2.ino`
   - Upload this simplified tracking-only demo
   - If this works, compare with main code

3. **Test Sensors Manually:**
   - Upload: `03 Tutorial & Code/03 SmartRobotCarV4.0_Tracking/TB6612/Demo1/` (sensor test)
   - Verify each sensor responds to black/white

---

**Last Updated:** February 12, 2026
**Status:** ✅ Code Fixed & Ready to Test

