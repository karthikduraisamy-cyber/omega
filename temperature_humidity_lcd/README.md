# Temperature and Humidity Monitor with Grove LCD Display

An Arduino Uno project that reads temperature and humidity from a DHT sensor and displays the results on a Grove RGB LCD display.

## Hardware Required

- Arduino Uno
- DHT11 or DHT22 Temperature and Humidity Sensor
- Grove RGB LCD Display (16x2)
- Jumper wires
- Breadboard (optional)

## Wiring Diagram

### DHT Sensor Connections
| DHT Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5V          |
| Data    | Digital Pin 2 |
| GND     | GND         |

**Note:** Some DHT sensors require a 10kΩ pull-up resistor between VCC and Data pin.

### Grove LCD Connections (I2C)
| Grove LCD | Arduino Pin |
|-----------|-------------|
| GND       | GND         |
| VCC       | 5V          |
| SDA       | A4          |
| SCL       | A5          |

**Note:** If using a Grove Base Shield, simply connect the LCD to any I2C port.

## Required Libraries

Install these libraries via Arduino IDE Library Manager (`Sketch > Include Library > Manage Libraries...`):

1. **DHT sensor library** by Adafruit
   - Search for "DHT sensor library" and install
   - Also install "Adafruit Unified Sensor" when prompted

2. **Grove - LCD RGB Backlight** by Seeed Studio
   - Search for "Grove LCD RGB Backlight"
   - Or download from: https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight

## Configuration

### Changing the DHT Sensor Type
If you're using a DHT22 instead of DHT11, change line 23:
```cpp
#define DHTTYPE DHT22     // Change from DHT11 to DHT22
```

### Changing the Data Pin
If you connected the DHT sensor to a different pin, change line 22:
```cpp
#define DHTPIN 4          // Change to your pin number
```

### Temperature in Fahrenheit
To display temperature in Fahrenheit, modify the `loop()` function:
```cpp
temperature = dht.readTemperature(true);  // true = Fahrenheit
```
And update the display line in `displayReadings()`:
```cpp
lcd.print("F");  // Instead of "C"
```

## Features

- **Continuous monitoring**: Updates every 2 seconds automatically
- **Dual display**: Shows both temperature and humidity
- **Trend tracking**: Displays arrows showing if values are:
  - ↑ Rising (increasing)
  - ↓ Falling (decreasing)  
  - = Stable (no change)
- **Color-coded backlight**: 
  - Blue: Cold (< 15°C)
  - Green: Comfortable (15-25°C)
  - Yellow: Warm (25-30°C)
  - Red: Hot (> 30°C)
- **Error handling**: Displays error message if sensor fails
- **Serial output**: Debug information sent to Serial Monitor at 9600 baud

## Troubleshooting

### "Sensor Error!" on LCD
- Check DHT sensor wiring
- Verify the correct pin is defined in code
- Ensure DHT sensor is properly powered (5V)
- Try adding a 10kΩ pull-up resistor

### LCD not displaying anything
- Check I2C connections (SDA to A4, SCL to A5)
- Verify LCD is powered (5V and GND)
- Run an I2C scanner sketch to detect the LCD address

### Incorrect readings
- DHT sensors need a few seconds to stabilize after power-on
- Ensure sensor is not placed near heat sources
- DHT11 has ±2°C accuracy; DHT22 has ±0.5°C accuracy

## License

This project is open source and available under the MIT License.
