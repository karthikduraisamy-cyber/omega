# Quick Sensor Calibration Reference

## Current Settings
Location: `ApplicationFunctionSet_xxx0.h` (line 70-72)

```cpp
uint8_t TrackingDetection_S = 250;   // Start of black line detection range
uint16_t TrackingDetection_E = 850;  // End of black line detection range  
uint16_t TrackingDetection_V = 950;  // Ground detection threshold (car lifted)
```

## What Each Sensor Should Read

### On Black Line (DETECTED - car should follow)
- Value should be: **250 to 850**
- Car action: Adjusts movement to stay on line

### On White Surface (NOT DETECTED - car searches)
- Value should be: **< 250 or > 850**
- Car action: Blind scan (oscillates to find line)

### Car Lifted Off Ground (SAFETY STOP)
- All three sensors: **> 950**
- Car action: STOP immediately

## Typical Sensor Readings

| Surface      | Typical Range | Your Readings | Status |
|--------------|---------------|---------------|--------|
| Black Line   | 300 - 700     | ___________   | ☐ OK   |
| White Surface| 850 - 1023    | ___________   | ☐ OK   |
| Car Lifted   | 1000 - 1023   | ___________   | ☐ OK   |

## Quick Calibration Steps

1. **Open Serial Monitor** (9600 baud)
2. **Activate tracking mode** (press button 1)
3. **Record values** in table above
4. **Adjust thresholds** if needed:

### If black line reads 300-700:
```cpp
TrackingDetection_S = 280;  // 20 below lowest
TrackingDetection_E = 720;  // 20 above highest
```

### If white surface reads 850-1000:
```cpp
TrackingDetection_V = 850;  // Just below white reading
```

## Test Commands

Put these in your Serial Monitor to test:

```
Place on white: All sensors should read > 850
Place middle on black: M should read 250-850, L and R > 850
Place right on black: R should read 250-850, L and M > 850
Place left on black: L should read 250-850, M and R > 850
Lift car: All sensors should read > 950
```

## Common Fixes

| Problem | Sensor Reading | Fix |
|---------|---------------|-----|
| Car doesn't move | All > 950 | Lower TrackingDetection_V to 1000 |
| Car ignores line | Values not in range | Adjust S and E to match readings |
| Car stops on white | White < 950 | Increase TrackingDetection_V |
| Erratic behavior | Range too wide | Narrow the S-E range |

## Hardware Checks
- [ ] ITR20001 module LEDs are ON
- [ ] Sensors are 2-3mm from ground
- [ ] Battery voltage > 7.0V
- [ ] Black line is matte (not shiny)
- [ ] Line width is 15-20mm
- [ ] Good contrast (black on white)

