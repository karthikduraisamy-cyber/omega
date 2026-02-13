# Servo Handwave for Arduino Uno

A servo motor program that performs an initial sweep and then continuously waves like a hand.

## Hardware Required

- Arduino Uno
- Grove Base Shield
- Servo Motor (standard servo like SG90 or MG90S)
- Grove Servo cable or jumper wires

## Wiring Diagram (Grove Base Shield)

| Servo Wire | Connection |
|------------|------------|
| Signal (Orange/Yellow) | Grove port **D5** |
| VCC (Red) | 5V |
| GND (Brown/Black) | GND |

> **Note:** If using a Grove Servo cable, just plug into port D5. For standard servo, connect signal wire to pin 5.

## Behavior

### Initial Sequence:
1. Starts at **0°** (1 second pause)
2. Moves to **90°** (1 second pause)
3. Moves to **180°** (1 second pause)

### Continuous Handwave:
- Waves smoothly between **45°** and **135°**
- Creates a natural hand-waving motion
- Loops forever

## Customization

### Change Wave Speed
```cpp
int waveSpeed = 15;    // Lower = faster, Higher = slower
```

### Change Wave Range
```cpp
int waveMin = 45;      // Minimum angle (0-180)
int waveMax = 135;     // Maximum angle (0-180)
```

### Change Servo Pin
```cpp
#define SERVO_PIN 6    // Change to D6, D7, or D8
```

## Troubleshooting

### Servo doesn't move
- Check power supply (servo needs good 5V)
- Verify signal wire is connected to correct pin
- Try a different Grove port

### Servo jitters
- Servo may need external power supply
- Try adding a capacitor across servo power lines

### Servo only moves partially
- Some servos have limited range (0-180° may not be fully supported)
- Adjust `waveMin` and `waveMax` values

## License

This project is open source and available under the MIT License.
