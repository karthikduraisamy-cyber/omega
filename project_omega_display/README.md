# Project Omega Display

A flashy animated display for the Grove RGB LCD showing "PROJECT OMEGA" and "AUTONOMOUS CAR" with colorful effects.

## Hardware Requirements

- Arduino Uno (or compatible board)
- Grove RGB LCD Display (16x2)

## Connections

| Grove LCD | Arduino |
|-----------|---------|
| SDA       | A4      |
| SCL       | A5      |
| VCC       | 5V      |
| GND       | GND     |

Or simply connect the Grove LCD to the I2C port on a Grove Base Shield.

## Required Libraries

- **Grove LCD RGB Backlight library** - Install via Arduino Library Manager or download from [Seeed Studio](https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight)

## Features

The display cycles through 5 different animation modes, each lasting 5 seconds:

1. **Rainbow Cycle** - Smooth color transitions through the entire spectrum with star decorations
2. **Flashing Colors** - Rapid color changes with arrow decorations
3. **Pulsing Effect** - Brightness fades in and out, changing colors at minimum brightness
4. **Scrolling Colors** - Wave-like color pattern shifts
5. **Strobe Effect** - Alternating display of each line with complementary colors

## Color Palette

The animation uses 8 vibrant colors:
- 🔴 Red
- 🟠 Orange  
- 🟡 Yellow
- 🟢 Green
- 🔵 Cyan
- 🔷 Blue
- 🟣 Purple
- 💗 Pink

## Startup Animation

When powered on, the display performs a character-by-character reveal animation with a color gradient, followed by a flash effect.

## Customization

### Change Display Text
Modify these lines in the code:
```cpp
const char* line1 = "PROJECT OMEGA";
const char* line2 = "AUTONOMOUS CAR";
```

### Adjust Animation Speed
- `colorChangeInterval` - Controls color transition speed (default: 100ms)
- `modeDuration` - How long each animation mode lasts (default: 5000ms)

### Add New Colors
Add to the `colors` array:
```cpp
const int colors[numColors][3] = {
  {255, 0, 0},     // Red
  // Add more {R, G, B} values here
};
```

## Upload Instructions

1. Connect the Arduino to your computer
2. Open `project_omega_display.ino` in Arduino IDE
3. Select the correct board and port
4. Click Upload

Enjoy your flashy autonomous car display! 🚗✨
