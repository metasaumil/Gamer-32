#include <BleGamepad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === OLED CONFIG ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === BLE GAMEPAD ===
BleGamepad bleGamepad("ESP32 Gamepad", "ESP32", 100);

// === Pins ===
// Analog Joysticks
#define JOY_LX 34
#define JOY_LY 35
#define JOY_RX 32
#define JOY_RY 33

// Joystick SW buttons (L3/R3)
#define SW_L 15  
#define SW_R 23  

// D-Pad
#define DPAD_UP     13
#define DPAD_DOWN   12
#define DPAD_LEFT   14
#define DPAD_RIGHT  27

// Action Buttons (A/B/X/Y mapped but no labels shown)
#define BTN_A 25  
#define BTN_B 26  
#define BTN_X 4   
#define BTN_Y 5   

// LED Pin
#define LED_PIN 2

// === Utility ===
int applyFullOnly(int value) {
  if (value >= 120) return 127;
  else if (value <= -120) return -127;
  else return 0;
}

// Mock battery percent (replace with real ADC logic)
int getMockBatteryPercent() {
  static int b = 95;
  b += (millis() / 5000) % 2 ? -1 : 1;
  if (b < 60) b = 95;
  return b;
}

void setup() {
  Serial.begin(115200);

  pinMode(DPAD_UP, INPUT_PULLUP);
  pinMode(DPAD_DOWN, INPUT_PULLUP);
  pinMode(DPAD_LEFT, INPUT_PULLUP);
  pinMode(DPAD_RIGHT, INPUT_PULLUP);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);

  pinMode(SW_L, INPUT_PULLUP);
  pinMode(SW_R, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  bleGamepad.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  Serial.println("Gamepad + OLED initialized.");
}

// Joystick circle with 90° clockwise rotation of position dot
void drawJoystickCircleRotated(int centerX, int centerY, int rawX, int rawY) {
  const int radius = 12;
  display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);

  // Map raw to -radius..radius before rotation
  int mappedX = map(rawX, 0, 4095, -radius, radius);
  int mappedY = map(rawY, 0, 4095, -radius, radius);

  // Apply rotation 90° clockwise: (x, y) -> (y, -x)
  float fx = mappedY;
  float fy = -mappedX;

  // Clamp to circle boundary (keeping margin of 2)
  float dist = sqrt(fx * fx + fy * fy);
  if (dist > radius - 2) {
    float scale = (radius - 2) / dist;
    fx *= scale;
    fy *= scale;
  }

  display.fillCircle(centerX + (int)fx, centerY + (int)fy, 2, SSD1306_WHITE);
}

void drawDpadVisual(int x, int y, int hat) {
  // Simple cross
  display.setTextSize(1);
  display.setCursor(x + 8, y);      display.print("^");
  display.setCursor(x, y + 8);      display.print("<");
  display.setCursor(x + 16, y + 8); display.print(">");
  display.setCursor(x + 8, y + 16); display.print("v");

  // Highlight current direction with small filled square
  if (hat == HAT_UP)              display.fillRect(x + 7, y + 1, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_DOWN)       display.fillRect(x + 7, y + 15, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_LEFT)       display.fillRect(x + 1, y + 7, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_RIGHT)      display.fillRect(x + 15, y + 7, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_UP_LEFT)    display.fillRect(x + 1, y + 1, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_UP_RIGHT)   display.fillRect(x + 15, y + 1, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_DOWN_LEFT)  display.fillRect(x + 1, y + 15, 3, 3, SSD1306_WHITE);
  else if (hat == HAT_DOWN_RIGHT) display.fillRect(x + 15, y + 15, 3, 3, SSD1306_WHITE);
}

void drawBatteryPercent(int percent) {
  char buf[6];
  snprintf(buf, sizeof(buf), "%d%%", percent);
  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(SCREEN_WIDTH - w - 1, 0);
  display.print(buf);
}

void drawStickButtonIndicator(int x, int y, bool pressed) {
  display.drawCircle(x, y, 6, SSD1306_WHITE);
  if (pressed) display.fillCircle(x, y, 3, SSD1306_WHITE);
}

void loop() {
  // Read raw joystick values
  int rawLX = analogRead(JOY_LX);
  int rawLY = analogRead(JOY_LY);
  int rawRX = analogRead(JOY_RX);
  int rawRY = analogRead(JOY_RY);

  // Map for BLE
  int LX = map(rawLX, 0, 4095, -127, 127);
  int LY = map(rawLY, 0, 4095, -127, 127);
  int RX = map(rawRX, 0, 4095, -127, 127);
  int RY = map(rawRY, 0, 4095, -127, 127);

  LX = applyFullOnly(LX);
  LY = applyFullOnly(LY);
  RX = applyFullOnly(RX);
  RY = applyFullOnly(RY);

  // BLE axes
  if (bleGamepad.isConnected()) {
    digitalWrite(LED_PIN, HIGH);
    bleGamepad.setAxes(LX, LY, RX, 0, 0, RY);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // D-Pad hat
  int hat = HAT_CENTERED;
  if (digitalRead(DPAD_UP) == LOW) {
    if (digitalRead(DPAD_RIGHT) == LOW) hat = HAT_UP_RIGHT;
    else if (digitalRead(DPAD_LEFT) == LOW) hat = HAT_UP_LEFT;
    else hat = HAT_UP;
  } else if (digitalRead(DPAD_DOWN) == LOW) {
    if (digitalRead(DPAD_RIGHT) == LOW) hat = HAT_DOWN_RIGHT;
    else if (digitalRead(DPAD_LEFT) == LOW) hat = HAT_DOWN_LEFT;
    else hat = HAT_DOWN;
  } else if (digitalRead(DPAD_RIGHT) == LOW) {
    hat = HAT_RIGHT;
  } else if (digitalRead(DPAD_LEFT) == LOW) {
    hat = HAT_LEFT;
  }
  bleGamepad.setHat(hat);

  // Action buttons
  bool aPressed = (digitalRead(BTN_A) == LOW);
  bool bPressed = (digitalRead(BTN_B) == LOW);
  bool xPressed = (digitalRead(BTN_X) == LOW);
  bool yPressed = (digitalRead(BTN_Y) == LOW);

  // Stick buttons
  bool l3Pressed = (digitalRead(SW_L) == LOW);
  bool r3Pressed = (digitalRead(SW_R) == LOW);

  // BLE button mapping
  aPressed ? bleGamepad.press(BUTTON_1) : bleGamepad.release(BUTTON_1);
  bPressed ? bleGamepad.press(BUTTON_2) : bleGamepad.release(BUTTON_2);
  xPressed ? bleGamepad.press(BUTTON_3) : bleGamepad.release(BUTTON_3);
  yPressed ? bleGamepad.press(BUTTON_4) : bleGamepad.release(BUTTON_4);
  l3Pressed ? bleGamepad.press(BUTTON_5) : bleGamepad.release(BUTTON_5);
  r3Pressed ? bleGamepad.press(BUTTON_6) : bleGamepad.release(BUTTON_6);

  // === OLED UI ===
  display.clearDisplay();

  // Top-left: S-BOX
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("S-BOX");

  // Top-center: pairing/connected
  const char *status = bleGamepad.isConnected() ? "Connected" : "Pairing";
  display.setTextSize(1);
  int16_t sx1, sy1;
  uint16_t sw, sh;
  display.getTextBounds(status, 0, 0, &sx1, &sy1, &sw, &sh);
  int statusX = (SCREEN_WIDTH - sw) / 2;
  display.setCursor(statusX, 0);
  display.print(status);

  // Top-right: battery
  drawBatteryPercent(getMockBatteryPercent());

  // Left: D-Pad
  drawDpadVisual(0, 20, hat);

  // Joysticks rotated, centered bottom
  drawJoystickCircleRotated(32, 52, rawLX, rawLY);   // left
  drawJoystickCircleRotated(96, 52, rawRX, rawRY);   // right

  // Action buttons diamond (no labels), mid-right
  int btnCenterX = 105;
  int btnCenterY = 32;
  // Top (Y)
  display.drawCircle(btnCenterX + 4, btnCenterY - 10, 5, SSD1306_WHITE);
  if (yPressed) display.fillCircle(btnCenterX + 4, btnCenterY - 10, 3, SSD1306_WHITE);
  // Bottom (A)
  display.drawCircle(btnCenterX + 4, btnCenterY + 6, 5, SSD1306_WHITE);
  if (aPressed) display.fillCircle(btnCenterX + 4, btnCenterY + 6, 3, SSD1306_WHITE);
  // Left (X)
  display.drawCircle(btnCenterX - 6, btnCenterY + 0, 5, SSD1306_WHITE);
  if (xPressed) display.fillCircle(btnCenterX - 6, btnCenterY + 0, 3, SSD1306_WHITE);
  // Right (B)
  display.drawCircle(btnCenterX + 14, btnCenterY + 0, 5, SSD1306_WHITE);
  if (bPressed) display.fillCircle(btnCenterX + 14, btnCenterY + 0, 3, SSD1306_WHITE);

  // Stick button indicators (L3/R3)
  drawStickButtonIndicator(32, 54, l3Pressed);
  drawStickButtonIndicator(96, 54, r3Pressed);

  display.display();

  // Serial debug
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 500) {
    Serial.printf("LX:%d LY:%d RX:%d RY:%d Hat:%d A:%d B:%d X:%d Y:%d L3:%d R3:%d\n",
                  map(rawLX, 0, 4095, -127, 127), map(rawLY, 0, 4095, -127, 127),
                  map(rawRX, 0, 4095, -127, 127), map(rawRY, 0, 4095, -127, 127),
                  hat, aPressed, bPressed, xPressed, yPressed, l3Pressed, r3Pressed);
    lastDebug = millis();
  }

  delay(30);
}
