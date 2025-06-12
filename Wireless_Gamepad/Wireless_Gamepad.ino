//with analog

#include <BleGamepad.h>

BleGamepad bleGamepad("ESP32 Gamepad", "ESP32", 100);

// Analog Joysticks
#define JOY_LX 34
#define JOY_LY 35
#define JOY_RX 32
#define JOY_RY 33

// Joystick SW buttons
#define SW_L 15  // Left stick click
#define SW_R 23  // Right stick click

// D-Pad
#define DPAD_UP     13
#define DPAD_DOWN   12
#define DPAD_LEFT   14
#define DPAD_RIGHT  27

// Action Buttons
#define BTN_CROSS    25  // Button 1
#define BTN_CIRCLE   26  // Button 2
#define BTN_SQUARE    4  // Button 3
#define BTN_TRIANGLE  5  // Button 4

// LED Pin
#define LED_PIN 2

int applyDeadzone(int value, int threshold = 10) {
  return (abs(value) < threshold) ? 0 : value;aaaaa
}

void setup() {
  Serial.begin(115200);

  // D-Pad
  pinMode(DPAD_UP, INPUT_PULLUP);
  pinMode(DPAD_DOWN, INPUT_PULLUP);
  pinMode(DPAD_LEFT, INPUT_PULLUP);
  pinMode(DPAD_RIGHT, INPUT_PULLUP);

  // Action buttons
  pinMode(BTN_CROSS, INPUT_PULLUP);
  pinMode(BTN_CIRCLE, INPUT_PULLUP);
  pinMode(BTN_SQUARE, INPUT_PULLUP);
  pinMode(BTN_TRIANGLE, INPUT_PULLUP);

  // Joystick buttons
  pinMode(SW_L, INPUT_PULLUP);
  pinMode(SW_R, INPUT_PULLUP);

  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  bleGamepad.begin();
  Serial.println("Gamepad Initialized.");
}

void loop() {
  if (bleGamepad.isConnected()) {
    digitalWrite(LED_PIN, HIGH);  // LED ON when connected

    // Analog sticks
    int rawLX = analogRead(JOY_LX);
    int rawLY = analogRead(JOY_LY);
    int rawRX = analogRead(JOY_RX);
    int rawRY = analogRead(JOY_RY);

    int LX = map(rawLX, 0, 4095, -127, 127);
    int LY = map(rawLY, 0, 4095, -127, 127);
    int RX = map(rawRX, 0, 4095, -127, 127);
    int RY = map(rawRY, 0, 4095, -127, 127);

    //Apply Deadzone
    LX = applyDeadzone(LX);
    LY = applyDeadzone(LY);
    RX = applyDeadzone(RX);
    RY = applyDeadzone(RY);
    
    bleGamepad.setAxes(LX, LY, RX, 0, 0, RY);

    // D-Pad handling
    int hat = HAT_CENTERED;
    String dpadState = "None";
    if (digitalRead(DPAD_UP) == LOW) {
      if (digitalRead(DPAD_RIGHT) == LOW) {
        hat = HAT_UP_RIGHT;
        dpadState = "UP + RIGHT";
      } else if (digitalRead(DPAD_LEFT) == LOW) {
        hat = HAT_UP_LEFT;
        dpadState = "UP + LEFT";
      } else {
        hat = HAT_UP;
        dpadState = "UP";
      }
    } else if (digitalRead(DPAD_DOWN) == LOW) {
      if (digitalRead(DPAD_RIGHT) == LOW) {
        hat = HAT_DOWN_RIGHT;
        dpadState = "DOWN + RIGHT";
      } else if (digitalRead(DPAD_LEFT) == LOW) {
        hat = HAT_DOWN_LEFT;
        dpadState = "DOWN + LEFT";
      } else {
        hat = HAT_DOWN;
        dpadState = "DOWN";
      }
    } else if (digitalRead(DPAD_RIGHT) == LOW) {
      hat = HAT_RIGHT;
      dpadState = "RIGHT";
    } else if (digitalRead(DPAD_LEFT) == LOW) {
      hat = HAT_LEFT;
      dpadState = "LEFT";
    }
    bleGamepad.setHat(hat);

    // Action buttons
    bool crossPressed    = (digitalRead(BTN_CROSS) == LOW);
    bool circlePressed   = (digitalRead(BTN_CIRCLE) == LOW);
    bool squarePressed   = (digitalRead(BTN_SQUARE) == LOW);
    bool trianglePressed = (digitalRead(BTN_TRIANGLE) == LOW);

    crossPressed    ? bleGamepad.press(BUTTON_1) : bleGamepad.release(BUTTON_1);
    circlePressed   ? bleGamepad.press(BUTTON_2) : bleGamepad.release(BUTTON_2);
    squarePressed   ? bleGamepad.press(BUTTON_3) : bleGamepad.release(BUTTON_3);
    trianglePressed ? bleGamepad.press(BUTTON_4) : bleGamepad.release(BUTTON_4);

    // Joystick buttons
    bool swLPressed = (digitalRead(SW_L) == LOW);
    bool swRPressed = (digitalRead(SW_R) == LOW);

    swLPressed ? bleGamepad.press(BUTTON_5) : bleGamepad.release(BUTTON_5);
    swRPressed ? bleGamepad.press(BUTTON_6) : bleGamepad.release(BUTTON_6);

    // Debug
    Serial.println("==== Gamepad State ====");
    Serial.printf("LX: %4d (raw: %4d)\tLY: %4d (raw: %4d)\n", LX, rawLX, LY, rawLY);
    Serial.printf("RX: %4d (raw: %4d)\tRY: %4d (raw: %4d)\n", RX, rawRX, RY, rawRY);
    Serial.printf("D-Pad: %s\n", dpadState.c_str());
    Serial.printf("Cross(X): %s\tCircle(O): %s\n", crossPressed ? "ON" : "OFF", circlePressed ? "ON" : "OFF");
    Serial.printf("Square(▢): %s\tTriangle(△): %s\n", squarePressed ? "ON" : "OFF", trianglePressed ? "ON" : "OFF");
    Serial.printf("SW_L (L Stick): %s\tSW_R (R Stick): %s\n",
                  swLPressed ? "ON" : "OFF",
                  swRPressed ? "ON" : "OFF");
    Serial.println("========================\n");

    delay(10);
  } else {
    // Blink LED when not connected
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}
