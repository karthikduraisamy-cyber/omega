# Sound Sensor Buzzer Alarm

An Arduino project that uses a Grove Sound Sensor to detect loud sounds and triggers a buzzer when the sound level exceeds a threshold (approximately 50 decibels).

## Hardware Required

- Arduino Uno
- Grove Base Shield
- Grove Sound Sensor
- Grove Buzzer

## Connections (Grove Base Shield)

| Component | Grove Port |
|-----------|------------|
| Sound Sensor | A0 (Analog) |
| Buzzer | D3 (Digital) |

## How It Works

1. The sound sensor continuously monitors ambient sound levels
2. When the detected sound exceeds the threshold, the buzzer activates for exactly 10 seconds
3. The buzzer stops after 10 seconds, then can be triggered again by another loud sound

## Calibration

The Grove Sound Sensor outputs analog values (0-1023) rather than direct decibel readings. The threshold value in the code (`SOUND_THRESHOLD = 500`) is set high enough to only trigger on loud sounds like clapping or shouting.

**To calibrate for your environment:**

1. Upload the sketch and open the Serial Monitor (9600 baud)
2. Observe the sound level readings in a quiet environment
3. Make sounds at different volumes and note the readings
4. Adjust the `SOUND_THRESHOLD` value as needed

## Serial Monitor Output

The program outputs real-time information to the Serial Monitor:
- Current sound level readings (every 500ms)
- Alerts when loud sounds are detected
- Buzzer state (ON/OFF)

## Usage

1. Connect the hardware as described above
2. Upload the sketch to your Arduino
3. Open Serial Monitor at 9600 baud to view readings
4. Clap or make loud sounds to trigger the buzzer!

## Customization

- **Threshold**: Modify `SOUND_THRESHOLD` to adjust sensitivity
- **Buzzer Duration**: Modify `BUZZER_DURATION` to change how long the buzzer stays on (default 10 seconds)
- **Print Interval**: Modify the print interval (currently 500ms) to see more or fewer readings
