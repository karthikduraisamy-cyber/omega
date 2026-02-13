# Project Omega - BIG Display with Flashy Animation

A flashy animated display program that shows "PROJECT" and "OMEGA" spread across both lines of a 16x2 LCD with eye-catching color-changing effects and animations.

## Features

- **Large Text Display**: "PROJECT" on line 1, "OMEGA" on line 2 - big and bold!
- **Epic Intro Animation**: 
  - Color sweep effect across the screen
  - Flash transitions
  - Letter-by-letter typing effect
  - Rainbow celebration
  - Sparkle effect

- **5 Rotating Display Effects**:
  1. **Smooth Rainbow Cycle** - Gentle color transitions through the spectrum
  2. **Strobe Flash** - Quick color changes for high energy
  3. **Pulse Breathing** - Colors fade in and out like breathing
  4. **Dual Color Alternating** - Magenta and Cyan switching
  5. **Fire Flicker** - Warm colors with random flickering like flames

- **Periodic Sparkle Animation**: Every 10 seconds, sparkles sweep across the display

## Hardware Requirements

- Arduino Uno (or compatible board)
- Grove RGB LCD Display (16x2 with RGB backlight)

## Wiring Connections

| Grove LCD | Arduino Uno |
|-----------|-------------|
| SDA       | A4          |
| SCL       | A5          |
| VCC       | 5V          |
| GND       | GND         |

*Note: If using a Grove Base Shield, simply connect the LCD to any I2C port.*

## Required Libraries

- **Grove LCD RGB Backlight** - For controlling the RGB LCD display
  - Install via Arduino Library Manager: Search for "Grove LCD RGB Backlight"

## Installation

1. Connect the Grove RGB LCD to your Arduino as shown above
2. Open `project_omega_big_display.ino` in Arduino IDE
3. Install the required library (Grove LCD RGB Backlight)
4. Select your board (Arduino Uno) and port
5. Upload the sketch

## How It Works

### Startup Sequence
1. A colorful block sweep fills the screen from left to right
2. Rapid flashing clears the display
3. "** PROJECT **" types in on line 1 with red backlight
4. "**  OMEGA  **" types in on line 2 with green backlight
5. Rainbow celebration flash
6. Sparkle effect across both lines

### Main Loop
The display cycles through 5 different lighting effects, changing every 5 seconds:
- Each effect creates a different mood and energy level
- A sparkle animation runs every 10 seconds for extra flair

## Customization

### Change Display Text
Modify these lines in the code:
```cpp
const char* line1Text = "  ** PROJECT **";
const char* line2Text = "  **  OMEGA  **";
```

### Adjust Effect Timing
- Effect duration: Change `5000` in `flashyDisplayLoop()` (milliseconds)
- Color change speed: Modify the delay values in each effect case
- Sparkle frequency: Change `10000` for text effect interval

### Add New Colors
Add to the `colors` array:
```cpp
const int colors[numColors][3] = {
  {255, 0, 0},     // Red
  // Add your RGB values here
};
```

## Troubleshooting

- **LCD not displaying**: Check I2C connections (SDA/SCL)
- **No backlight colors**: Verify the Grove LCD has RGB capability
- **Garbled text**: Reset Arduino and check library installation

## License

Open source for educational purposes.
