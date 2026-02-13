# Rotary Angle Sensor Servo Control

Control a servo motor using a rotary angle sensor (potentiometer). The servo angle follows the knob position in real-time - turn right to move the servo right!

## Hardware Required

- Arduino Uno
- Grove Base Shield
- Grove Rotary Angle Sensor
- Servo Motor (standard hobby servo)

## Connections

| Component | Grove Port | Pin |
|-----------|------------|-----|
| Rotary Angle Sensor | A0 | Analog Pin A0 |
| Servo Motor | D5 | Digital Pin 5 |

### Wiring Diagram

```
Arduino Uno + Grove Base Shield
         ┌─────────────────┐
         │                 │
    A0 ──┤  Rotary Sensor  │ (Turn knob to control)
         │                 │
         └─────────────────┘
         ┌─────────────────┐
         │                 │
    D5 ──┤   Servo Motor   │ (Follows knob position)
         │                 │
         └─────────────────┘
```

## How It Works

1. **Rotary Sensor Input**: The Grove rotary angle sensor is a potentiometer with approximately 300° of rotation. It outputs an analog voltage proportional to the knob position.

2. **Analog Reading**: The Arduino reads the sensor value (0-1023) from the analog pin.

3. **Angle Mapping**: The sensor value is mapped to a servo angle (0-180°):
   - Knob fully left (counter-clockwise) → Servo at 0°
   - Knob fully right (clockwise) → Servo at 180°

4. **Smoothing**: A moving average filter reduces noise and prevents servo jitter.

5. **Threshold**: Small changes are ignored to prevent unnecessary servo movement.

## Features

- **Real-time sync**: Servo position matches knob position instantly
- **Noise filtering**: Moving average smoothing for stable operation
- **Jitter prevention**: Threshold-based updates reduce servo chatter
- **Serial monitoring**: View sensor and angle values in Serial Monitor

## Serial Output

Open the Serial Monitor at 9600 baud to see:
```
Rotary Angle Sensor Servo Control
==================================
Turn the knob to control the servo!

Ready! Rotate the knob to move the servo.
Sensor: 512 | Knob Angle: 150° | Servo Angle: 90°
Sensor: 768 | Knob Angle: 225° | Servo Angle: 135°
```

## Customization

You can adjust these parameters in the code:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SMOOTHING_FACTOR` | 5 | Number of readings to average (higher = smoother but slower) |
| `UPDATE_DELAY` | 15 | Milliseconds between updates |
| `angleThreshold` | 2 | Minimum angle change to trigger servo update |

## Troubleshooting

1. **Servo not moving**: Check connections and ensure servo is powered
2. **Erratic movement**: Increase `SMOOTHING_FACTOR` or `angleThreshold`
3. **Slow response**: Decrease `SMOOTHING_FACTOR` or `UPDATE_DELAY`
4. **Wrong direction**: If servo moves opposite to knob, swap the map values:
   ```cpp
   int servoAngle = map(sensorValue, 0, ADC_MAX, SERVO_MAX_ANGLE, SERVO_MIN_ANGLE);
   ```

## Notes

- The Grove Rotary Angle Sensor has a 300° rotation range
- Standard servo motors have a 180° range
- The mapping scales the full knob rotation to the full servo range
