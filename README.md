# Interactive Garden

An interactive learning toy for first and second grade children featuring RFID technology and colorful LED feedback. This project creates a game board where children can place objects with embedded RFID tags on specific positions, with the system providing visual feedback through LED rings.

## Project Overview

The Interactive Garden uses multiple RFID readers positioned under a game board to detect objects placed on different positions. Each object contains a unique RFID tag that identifies it to the system. When a tag is detected, an LED ring lights up with a specific color corresponding to that tag.

## Components Required

- Arduino Uno or compatible board
- 2+ MFRC522 RFID readers (expandable system)
- RFID tags/cards (compatible with MFRC522)
- NeoPixel LED ring with 12 LEDs (WS2812B-based)
- Jumper wires
- Optional: 100-330 ohm resistor (for LED data line)
- Optional: 470-1000μF capacitor (for LED power stability)
- Breadboard or custom PCB for connections

## Libraries Required

- **SPI.h**: For communication with RFID readers
- **MFRC522**: For controlling the RFID readers [(Library Link)](https://github.com/miguelbalboa/rfid)
- **Adafruit_NeoPixel**: For controlling the LED ring [(Library Link)](https://github.com/adafruit/Adafruit_NeoPixel)

## Installation Instructions

1. **Install Arduino IDE** if not already installed
2. **Install Required Libraries**:
   - Go to Sketch > Include Library > Manage Libraries
   - Search for and install "MFRC522" 
   - Search for and install "Adafruit NeoPixel"
3. **Wire the components** according to the wiring diagram below
4. **Upload the code** from reader.ino to your Arduino

## Wiring Diagram

### RFID Reader 1:
- VCC → 3.3V on Arduino
- RST → Pin 9 on Arduino
- GND → GND on Arduino
- MISO → Pin 12 on Arduino
- MOSI → Pin 11 on Arduino
- SCK → Pin 13 on Arduino
- SDA/SS → Pin 10 on Arduino

### RFID Reader 2:
- VCC → 3.3V on Arduino
- RST → Pin 8 on Arduino
- GND → GND on Arduino
- MISO → Pin 12 on Arduino (shared with Reader 1)
- MOSI → Pin 11 on Arduino (shared with Reader 1)
- SCK → Pin 13 on Arduino (shared with Reader 1)
- SDA/SS → Pin 7 on Arduino

### NeoPixel LED Ring:
- DI (Data In) → Pin 5 on Arduino (optionally through a 100-330 ohm resistor)
- 5V → 5V on Arduino
- GND → GND on Arduino
- DO (Data Out) → Not connected

## Code Description

The Arduino sketch in reader.ino performs the following functions:

1. **Initialization**: Sets up RFID readers and LED ring
2. **Tag Detection**: Continuously scans for RFID tags on both readers
3. **Color Response**: 
   - Green tag (UID: CA 6E EF 3F) → Green LED ring
   - Red tag (UID: 76 43 42 29) → Red LED ring
   - Unknown tags → Blue LED ring with initial rainbow animation
4. **Persistence**: LEDs remain lit as long as a tag is detected, turning off shortly after tag removal

## Adding More Readers

The system can be expanded with additional RFID readers by:
1. Connecting each reader's SDA/SS pin to a unique digital pin on the Arduino
2. Connecting RST to a unique pin
3. Sharing the MISO, MOSI, and SCK connections
4. Adding the reader initialization and checking code in the Arduino sketch

## Troubleshooting

- **LEDs not lighting**: Check power supply and data pin connection
- **Inconsistent tag reading**: Ensure proper spacing between readers to avoid interference
- **LED flicker**: Add a capacitor between 5V and GND near the LED ring
- **Arduino resetting**: Reduce LED brightness or use external power supply

## Future Enhancements

- Multiple LED rings for more complex feedback
- Sound feedback to accompany visual cues
- Game logic for educational activities
- Serial communication with a computer for advanced interactions