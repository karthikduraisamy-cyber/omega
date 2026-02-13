# LED Button Control for Arduino Uno

A simple Arduino program that lights up an LED when you press a button using Grove Base Shield.

## Hardware Required

- Arduino Uno
- Grove Base Shield
- Grove LED Socket Module
- Grove Button Module
- Grove cables

## Wiring Diagram (Grove Base Shield)

### Connections
| Grove Module | Grove Port |
|--------------|------------|
| LED Socket   | **D4**     |
| Button       | **D3**     |

Simply plug the Grove modules into the corresponding ports on the Base Shield!

## How It Works

1. Grove Button outputs HIGH when pressed, LOW when not pressed
2. When button is **not pressed**: Pin reads LOW, LED is OFF
3. When button is **pressed**: Pin reads HIGH, LED is ON
4. Small delay (10ms) helps with button debouncing

## Behavior

- **Press and hold** the button → LED stays ON
- **Release** the button → LED turns OFF

## Customization

### Change LED Port
```cpp
#define LED_PIN 5    // Change to D5, D6, D7, or D8
```

### Change Button Port
```cpp
#define BUTTON_PIN 2    // Change to D2, D5, D6, D7, or D8
```

### Toggle Mode (press to toggle ON/OFF)
Replace the `loop()` function with:
```cpp
bool ledState = false;
bool lastButtonState = HIGH;

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  // Detect button press (transition from HIGH to LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState;  // Toggle LED state
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    delay(50);  // Debounce delay
  }
  
  lastButtonState = buttonState;
}
```

## Troubleshooting

### LED doesn't light up
- Check Grove LED module is plugged into port **D4**
- Ensure Grove cable is fully connected
- Verify the LED is properly seated in the socket

### Button doesn't respond
- Check Grove Button module is plugged into port **D3**
- Test with Serial Monitor (open at 9600 baud)
- Try pressing the button firmly

## License

This project is open source and available under the MIT License.
