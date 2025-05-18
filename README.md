# Interactive Garden

An interactive learning toy for first and second grade children featuring RFID technology and colorful LED feedback. This project creates a game board where children can place objects with embedded RFID tags on specific positions, with the system providing visual feedback through LED rings.

## Project Overview

The Interactive Garden uses multiple RFID readers positioned under a game board to detect objects placed on different positions. Each object contains a unique RFID tag that identifies it to the system. When a tag is detected, an LED ring lights up with a specific color corresponding to that tag.

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

### NeoPixel LED Ring 1:
- DI (Data In) → Pin 5 on Arduino (optionally through a 100-330 ohm resistor)
- 5V → 5V on Arduino
- GND → GND on Arduino
- DO (Data Out) → Not connected

### NeoPixel LED Ring 2:
- DI (Data In) → Pin 6 on Arduino (optionally through a 100-330 ohm resistor)
- 5V → 5V on Arduino
- GND → GND on Arduino
- DO (Data Out) → Not connected

## Code Description

The Arduino sketch performs the following functions:

1. **Initialization**: Sets up RFID readers and LED rings
2. **Tag Detection**: Continuously scans for RFID tags on both readers
3. **Color Response**: 
   - Green tag (UID: CA 6E EF 3F) → Green LED on corresponding ring
   - Red tag (UID: 76 43 42 29) → Red LED on corresponding ring
   - Unknown tags → Blue LED on corresponding ring with initial rainbow animation
4. **Persistence**: LEDs remain lit as long as a tag is detected, turning off shortly after tag removal
5. **Independent Operation**: Each reader controls its own LED ring, allowing for simultaneous tag detection

## Adding More Readers and Rings

The system can be expanded with additional RFID readers and LED rings by:
1. Connecting each reader's SDA/SS pin to a unique digital pin on the Arduino
2. Connecting each reader's RST to a unique pin
3. Connecting each LED ring's data input to a unique pin
4. Sharing the MISO, MOSI, and SCK connections between readers
5. Adding the reader and ring initialization and checking code in the Arduino sketch

## Troubleshooting

- **LEDs not lighting**: Check power supply and data pin connections
- **Inconsistent tag reading**: Ensure proper spacing between readers to avoid interference
- **LED flicker**: Add a capacitor between 5V and GND near the LED rings
- **Arduino resetting**: Reduce LED brightness or use external power supply for LEDs
- **Power issues**: When using multiple LED rings, consider using an external 5V power supply

## Future Enhancements

- Additional LED rings for more complex feedback
- Sound feedback to accompany visual cues
- Game logic for educational activities
- Serial communication with a computer for advanced interactions
- Customizable color patterns for different learning activities