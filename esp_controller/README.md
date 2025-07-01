# Interactive Garden ESP Controller

This ESP8266/ESP32 controller creates a WiFi access point and provides a REST API to control a WS2812B LED strip for the Interactive Garden project.

## Hardware Requirements

- ESP8266 (NodeMCU or similar) or ESP32 development board
- WS2812B LED strip
- 5V power supply appropriate for your LED strip length
- Optional: 1000μF capacitor between power supply and LED strip
- Optional: 300-500 ohm resistor on the data line

## Wiring

1. Connect the LED strip to the ESP:
   - LED strip GND → ESP GND
   - LED strip VCC → 5V power supply
   - LED strip DIN → ESP D4 (GPIO2) through a 330-500 ohm resistor

2. Power supply:
   - Connect the power supply GND to both the ESP and LED strip GND
   - Connect the power supply 5V to the LED strip VCC
   - For longer LED strips, place a 1000μF capacitor between power supply and LED strip

## Software Setup

1. Install the required Arduino libraries:
   - ESP8266WiFi (for ESP8266) or WiFi (for ESP32)
   - ESP8266WebServer (for ESP8266) or WebServer (for ESP32)
   - ArduinoJson
   - FastLED

2. Adjust the configuration in the code:
   - Set the correct `LED_PIN` for your board
   - Update `NUM_LEDS` to match your LED strip length
   - Change the `ssid` and `password` if desired

3. Upload the code to your ESP device

## Usage

1. Connect to the WiFi access point named "InteractiveGarden" (password: garden1234)
2. Access the web interface at http://192.168.4.1
3. The LED strip can be controlled via the REST API endpoints:

### API Endpoints

- `POST /api/led`: Control LED segments
  - Body: JSON object with `start`, `end`, and `color` properties
  - Example: 
    ```json
    {
      "start": 0,
      "end": 5,
      "color": {"r": 255, "g": 0, "b": 0}
    }
    ```

- `POST /api/clear`: Turn off all LEDs
  - No body required

## Integration with Web Application

The ESP controller is designed to work with the Interactive Garden web application. The web application can be configured to send LED control commands to the ESP when plants are placed or evaluated on the grid.