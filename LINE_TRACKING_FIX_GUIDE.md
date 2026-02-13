# Line Tracking Function Fix Guide

## Problem Identified
The line tracking function was not working properly due to several issues:

1. **Sensor reading issue**: The tracking function was using cached sensor values from `ApplicationFunctionSet_SensorDataUpdate()` instead of reading fresh values
2. **Debug output disabled**: No way to see what values the sensors were actually reading
3. **Potential threshold mismatch**: Detection range (250-850) might not match your specific sensors and track

## Changes Made

### 1. Enabled Debug Output
Added `#define _Test_print 1` to enable serial debug output showing:
- Left, Middle, and Right sensor values
- Ground detection status
- Detection range being used

### 2. Fresh Sensor Reading
Modified the tracking function to read sensor values directly when needed, like the working demo code does.

### 3. Improved Debug Messages
Enhanced debug output to show all critical values in one line for easier troubleshooting.

## How to Test

### Step 1: Upload and Monitor
1. Upload the modified code to your Arduino
2. Open Serial Monitor (Tools → Serial Monitor) at 9600 baud
3. Press button 1 on the car to activate Line Tracking mode
4. Watch the sensor values being printed every 500ms

### Step 2: Calibrate Sensor Thresholds
The debug output will show values like:
```
ITR20001: L=245 M=520 R=312 | Ground=YES | Range=[250-850]
```

**Understanding the values:**
- **Black line**: Sensors should read values in the range [250-850]
- **White surface**: Sensors should read values outside this range (usually > 950 or < 250)
- **Ground=NO**: Means all sensors read > 950 (car lifted or all on white)

### Step 3: Adjust if Needed
If your sensors read different values, modify these lines in `ApplicationFunctionSet_xxx0.h`:

```cpp
uint8_t TrackingDetection_S = 250;   // Lower threshold - adjust based on your readings
uint16_t TrackingDetection_E = 850;  // Upper threshold - adjust based on your readings
uint16_t TrackingDetection_V = 950;  // Ground detection threshold
```

**Example adjustments:**
- If black line reads 300-700, set: `S=280, E=720`
- If white surface reads 800-1000, set: `V=850`

## Common Issues and Solutions

### Issue 1: Car doesn't move at all in tracking mode
**Cause**: All sensors reading > 950 (ground detection triggered)
**Solution**: 
- Check sensor connections
- Verify sensors are facing down and close to ground (about 2-3mm)
- Adjust `TrackingDetection_V` value

### Issue 2: Car doesn't follow the line properly
**Cause**: Detection range doesn't match sensor readings
**Solution**:
- Place car on black line
- Check debug output for sensor values on black line
- Adjust `TrackingDetection_S` and `TrackingDetection_E` to match

### Issue 3: Car stops immediately when activated
**Cause**: Ground detection thinks car is lifted
**Solution**:
- Verify sensor values on white surface
- If readings are close to 950, increase `TrackingDetection_V` to 1000

## Testing Checklist

- [ ] Serial Monitor shows sensor values updating
- [ ] Sensor values change when moved over black line
- [ ] Middle sensor detects line (value in range) → Car moves forward
- [ ] Right sensor detects line → Car turns right
- [ ] Left sensor detects line → Car turns left
- [ ] Car doesn't stop when on white surface (unless timeout)
- [ ] "Blind scan" works when line is lost (oscillates left-right to find line)

## Advanced Troubleshooting

### Verify Sensor Hardware
Run this simple test in Serial Monitor with debug enabled:
1. Lift car off ground → All values should be > 950
2. Place on white surface → All values should be > 850
3. Place middle sensor on black line → Middle value should be 250-850
4. Move line to right sensor → Right value should be 250-850
5. Move line to left sensor → Left value should be 250-850

### Check Track Quality
- Line should be **black** on **white** background
- Line width: 15-20mm (wider is better)
- Use **matte** black tape or paper (shiny surfaces cause issues)
- Ensure good contrast between line and background
- Avoid shadows or strong lighting variations

## Disable Debug Output
Once everything works, disable debug output to improve performance:
In `ApplicationFunctionSet_xxx0.cpp`, change:
```cpp
#define _Test_print 1  // Enable debug printing
```
to:
```cpp
#define _Test_print 0  // Disable debug printing
```

## Contact for Help
If issues persist after following this guide:
1. Note down the sensor values from Serial Monitor
2. Take a photo of your track
3. Check if the ITR20001 module LEDs are on (should be visible when powered)
4. Verify power supply voltage (should be > 7.0V)

