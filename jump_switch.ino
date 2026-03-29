/*
 * JUMP SWITCH — Keep jumping to keep the music going
 */

// ── PIN CONFIGURATION ──
const int jumpPin = 2;   // wire from shoe foil connects here
const int ledPin = 13;   // built-in LED gives visual feedback on landing

// ── STATE TRACKING ──
// Stores the previous state of the switch so we can detect changes.
// Starts as HIGH because INPUT_PULLUP holds the pin HIGH when the circuit is open.
int lastSwitchState = HIGH;

// ── SETUP ──
void setup() {
  // INPUT_PULLUP activates an internal ~50kΩ resistor inside the Arduino.
  // This holds Pin 2 at HIGH (5V) by default so the pin is never left floating.
  // When the foil patches touch, the circuit connects Pin 2 to GND, pulling it LOW.
  // This means no external resistor is needed.
  pinMode(jumpPin, INPUT_PULLUP);

  // Set the built-in LED as an output so we can turn it on and off
  pinMode(ledPin, OUTPUT);

  // Start serial communication at 9600 baud to talk to the Python script
  Serial.begin(9600);

  // Confirm the board is running and ready
  Serial.println("Ready.");
}

// ── MAIN LOOP ──
void loop() {
  // Read the current state of the jump switch
  // LOW = foil patches are touching (landed)
  // HIGH = foil patches are separated (in the air)
  int currentSwitchState = digitalRead(jumpPin);

  // Only act if the state has changed since the last loop
  if (currentSwitchState != lastSwitchState) {

    // Wait 50ms before reading again — debouncing — one landing could trigger dozens of false readings.
    delay(50);
    currentSwitchState = digitalRead(jumpPin);

    // Re-check that the state still differs after the debounce delay.
    // If the reading went back to the original state, it was just noise — ignore it.
    if (currentSwitchState != lastSwitchState) {

      if (currentSwitchState == LOW) {
        // Foil patches are touching — the circuit is complete — user has landed
        digitalWrite(ledPin, HIGH);       // turn LED on as visual confirmation
        Serial.println("LANDED");        // notify the Python script to start music
      } else {
        // Foil patches are separated — the circuit is open — user is in the air
        digitalWrite(ledPin, LOW);        // turn LED off
        Serial.println("JUMP");          // notify the Python script
      }

      // Save the current state so we can detect the next change
      lastSwitchState = currentSwitchState;
    }
  }
}