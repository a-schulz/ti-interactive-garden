/**
 * Button Test Program
 * 
 * This program tests a button connected to digital pin 2.
 * It will print a message to the serial monitor whenever the button is pressed.
 * This helps verify that your button wiring is correct and debouncing is working.
 * 
 * Wiring for 4-leg tactile button:
 * - Connect one leg pair to digital pin 2
 * - Connect the other leg pair to GND
 * 
 * The code uses Arduino's internal pull-up resistor, so no external resistor is needed.
 */

#include <Arduino.h>

// Button pin
#define BUTTON_PIN 2

// Button state variables
int buttonState = HIGH;         // Current state of the button (HIGH = not pressed with pull-up)
int lastButtonState = HIGH;     // Previous state of the button
unsigned long lastDebounceTime = 0;  // Last time the button state changed
unsigned long debounceDelay = 50;    // Debounce time in ms

// Button press counter
int buttonPresses = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while(!Serial && millis() < 3000); // Wait for serial connection to establish
  
  // Initialize button pin with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Button Test Program");
  Serial.println("Press the button connected to pin 2 to see if it works");
  Serial.println("If wired correctly, 'Button Pressed!' will appear when you press the button");
}

void loop() {
  // Read the current button state
  int reading = digitalRead(BUTTON_PIN);
  
  // If the button state changed, reset the debounce timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // If enough time has passed since the last state change, check if button state really changed
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed
    if (reading != buttonState) {
      buttonState = reading;
      
      // If the button is pressed (LOW due to pull-up resistor)
      if (buttonState == LOW) {
        buttonPresses++;
        Serial.print("Button Pressed! Count: ");
        Serial.println(buttonPresses);
        
        // Show which legs to use for 4-leg button
        Serial.println("For 4-leg buttons, if this works you've connected the correct legs.");
        Serial.println("Remember to connect one leg from each pair (e.g., legs 1 & 3 or 2 & 4)");
        
        // Visual indicator that we received the press - blink built-in LED
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }
  
  // Save the current reading for next comparison
  lastButtonState = reading;
  
  // Small delay to prevent CPU hogging
  delay(10);
}