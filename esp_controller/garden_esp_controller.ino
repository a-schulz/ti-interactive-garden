#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FastLED.h>

// FastLED configuration
#define LED_PIN     D13 // GPIO2 (D13) on NodeMCU
#define NUM_LEDS    72 // Increased to support 6x6 grid (72 LEDs)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// WiFi configuration
const char* ssid = "InteractiveGarden";
const char* password = "garden1234";

// Create web server and LED array
ESP8266WebServer server(80);
CRGB leds[NUM_LEDS];

// CORS headers for web browser access
void setCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  setCorsHeaders();
  server.send(200);
}

// Handle LED control endpoint
void handleLedControl() {
  setCorsHeaders();
  
  // Check if the request has a body
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Bad Request: Missing body");
    return;
  }
  
  // Parse the JSON request
  String body = server.arg("plain");
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "text/plain", "Bad Request: Invalid JSON");
    return;
  }
  
  // Extract parameters
  int start = doc["start"];
  int end = doc["end"];
  int r = doc["color"]["r"];
  int g = doc["color"]["g"];
  int b = doc["color"]["b"];
  
  // Validate parameters
  if (start < 0 || start >= NUM_LEDS || end < 0 || end >= NUM_LEDS || start > end) {
    server.send(400, "text/plain", "Bad Request: Invalid LED range");
    return;
  }
  
  // Set LED colors
  for (int i = start; i <= end; i++) {
    leds[i] = CRGB(r, g, b);
  }
  FastLED.show();
  
  // Send success response
  server.send(200, "application/json", "{\"success\":true,\"message\":\"LEDs updated\"}");
}

// Handle clearing all LEDs
void handleClearLeds() {
  setCorsHeaders();
  
  FastLED.clear();
  FastLED.show();
  
  server.send(200, "application/json", "{\"success\":true,\"message\":\"All LEDs cleared\"}");
}

// Handle home page
void handleRoot() {
  setCorsHeaders();
  
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Interactive Garden Controller</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; text-align: center; }";
  html += "h1 { color: #2e7d32; }";
  html += ".container { max-width: 800px; margin: 0 auto; }";
  html += ".status { background-color: #f1f8e9; padding: 15px; border-radius: 5px; margin: 20px 0; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>Interactive Garden LED Controller</h1>";
  html += "<div class='status'>";
  html += "<p>Status: Running</p>";
  html += "<p>LED Count: " + String(NUM_LEDS) + "</p>";
  html += "<p>Access Point: " + String(ssid) + "</p>";
  html += "</div>";
  html += "<p>This ESP controller provides a REST API to control the LED strip for the Interactive Garden project.</p>";
  html += "<p>Use the web application to interact with the garden or send POST requests directly to /api/led endpoint.</p>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("\nInteractive Garden ESP Controller");
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50); // Set brightness (0-255)
  FastLED.clear();
  FastLED.show();
  
  // Setup WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Define API routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/led", HTTP_POST, handleLedControl);
  server.on("/api/clear", HTTP_POST, handleClearLeds);
  server.on("/api/led", HTTP_OPTIONS, handleOptions);
  server.on("/api/clear", HTTP_OPTIONS, handleOptions);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  
  // Show startup animation on LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(20);
  }
  delay(500);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Handle client requests
  server.handleClient();
}