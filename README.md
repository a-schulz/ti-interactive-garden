# Interactive Garden

Children toy for kids first and second grade.
RFID Reader below each position on a game board. RFID Tags in every object to be placed on the positions. Controller detects which object is placed on which position. It reacts by turning on a specific color on placed led rings.


## Components Required
- Arduino Uno
- RFID Reader (e.g., MFRC522)
- RFID Chips
- Jumper wires
- Breadboard (optional)

## Libraries Used
- **MFRC522**: This library is used to interface with the MFRC522 RFID reader. It provides functions to initialize the reader, read RFID tags, and manage communication between the Arduino and the RFID reader.

## Installation Instructions
1. **Open the Arduino IDE**.
2. **Go to Sketch > Include Library > Manage Libraries**.
3. In the Library Manager, search for "MFRC522".
4. Click on the "Install" button to add the library to your Arduino IDE.


## Wiring Diagram for Two RFID Readers with One LED Ring
### RFID Reader 1 (original):
VCC → 3.3V on Arduino
RST → Pin 9 on Arduino
GND → GND on Arduino
MISO → Pin 12 on Arduino (MISO pin)
MOSI → Pin 11 on Arduino (MOSI pin)
SCK → Pin 13 on Arduino (SCK pin)
SDA/SS → Pin 10 on Arduino

### RFID Reader 2 (new):
VCC → 3.3V on Arduino
RST → Pin 8 on Arduino
GND → GND on Arduino
MISO → Pin 12 on Arduino (share MISO pin)
MOSI → Pin 11 on Arduino (share MOSI pin)
SCK → Pin 13 on Arduino (share SCK pin)
SDA/SS → Pin 7 on Arduino

### NeoPixel LED Ring:
DI (Data In) → Pin 5 on Arduino (optionally through a 100-330 ohm resistor)
5V → 5V on Arduino
GND → GND on Arduino (can share ground with RFID modules)
DO (Data Out) → Not connected

