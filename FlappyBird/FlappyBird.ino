#include <TFT_eSPI.h>
#include <SPI.h>

#define BUTTON_PIN 15

TFT_eSPI tft = TFT_eSPI();
int birdY = 60;
float velocity = 0;
const float gravity = 0.5;
const int flapStrength = -4;
int pipeX = 240, gapY = 60;
const int gapSize = 40;
int score = 0;
bool scored = false;  // to prevent double scoring

int oldBirdY = birdY;
int oldPipeX = pipeX;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 30; // ~30fps

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  tft.init();
  tft.setRotation(3);  // Set to landscape mode
  tft.fillScreen(TFT_BROWN);
  tft.setRotation(1);  // Set to landscape mode
  tft.fillScreen(TFT_CYAN);
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.setTextSize(2);

  // Initial score draw
  tft.setCursor(10, 10);
  tft.print("Score: 0");
}

void drawBird() {
  tft.fillCircle(30, birdY, 5, TFT_YELLOW);
}

void drawPipe(int x, int gapY) {
  tft.fillRect(x, 0, 20, gapY, TFT_GREEN);
  tft.fillRect(x, gapY + gapSize, 20, 240 - (gapY + gapSize), TFT_GREEN);
}

void updateScore() {
  // Only score if bird has passed the pipe and hasn't already scored for it
  if (!scored && pipeX + 20 < 30 - 5) {
    score++;
    scored = true;

    // Erase old score area
    tft.fillRect(10, 10, 100, 20, TFT_CYAN);

    // Redraw score at the top, so it's always in front
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_BLACK, TFT_CYAN);
    tft.print("Score: ");
    tft.print(score/2);
  }
}

void loop() {
  // Update only if enough time has passed
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();

    // Input
    if (digitalRead(BUTTON_PIN) == LOW) {
      velocity = flapStrength;
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else digitalWrite(LED_BUILTIN, LOW);

    // === 1. Erase previous bird and pipes ===
    tft.fillCircle(30, oldBirdY, 5, TFT_CYAN);
    tft.fillRect(oldPipeX, 0, 20, gapY, TFT_CYAN);
    int lowerPipeHeight = 240 - (gapY + gapSize);
    tft.fillRect(oldPipeX, gapY + gapSize, 20, lowerPipeHeight, TFT_CYAN);

    // === 2. Physics and pipe movement ===
    velocity += gravity;
    birdY += (int)velocity;
    pipeX -= 2;

    // Update score when pipe passes bird
    if (pipeX == 30) score++;

    if (pipeX < -20) {
      pipeX = 240;
      gapY = random(30, 150);
      scored = false;  // reset for new pipe
    }

    // === 3. Collision detection ===
    if (
      birdY > 240 || birdY < 0 ||
      (pipeX < 35 && pipeX + 20 > 25 &&
       (birdY - 5 < gapY || birdY + 5 > gapY + gapSize))
    ) {
      tft.setRotation(3);  // Set to landscape mode
      tft.fillScreen(TFT_RED);
      tft.setRotation(1);  // Set to landscape mode
      tft.fillScreen(TFT_RED);
      tft.setCursor(70, 150);
      tft.print("GAME OVER");
      delay(2000);
      birdY = 60; pipeX = 240; velocity = 0; score = 0;
      tft.setRotation(3);  // Set to landscape mode
      tft.fillScreen(TFT_BROWN);
      tft.setRotation(1);  // Set to landscape mode
      tft.fillScreen(TFT_CYAN);
      oldBirdY = birdY; oldPipeX = pipeX;
      tft.setCursor(10, 10);
      tft.print("Score: 0");
      return;
    }

    // === 4. Draw new positions ===
    drawBird();
    drawPipe(pipeX, gapY);

    // === 5. Update the score (ensure it is always in front) ===
    updateScore();

    oldBirdY = birdY;
    oldPipeX = pipeX;
  }
}

// void loop() {
//   // Input
//   if (digitalRead(BUTTON_PIN) == LOW) {
//     velocity = flapStrength;
//     digitalWrite(LED_BUILTIN, HIGH);
//   }
//   digitalWrite(LED_BUILTIN, LOW);

//   // Erase previous bird and pipes
//   tft.fillCircle(30, oldBirdY, 5, TFT_CYAN);
//   tft.fillRect(oldPipeX, 0, 20, gapY, TFT_CYAN);
//   int lowerPipeHeight = 240 - (gapY + gapSize);
//   tft.fillRect(oldPipeX, gapY + gapSize, 20, lowerPipeHeight, TFT_CYAN);

//   // Physics
//   velocity += gravity;
//   birdY += (int)velocity;

//   // Move pipe
//   pipeX -= 2;
//   if (pipeX < -20) {
//     pipeX = 240;
//     gapY = random(30, 150);
//     scored = false;  // reset for new pipe
//   }

//   updateScore();

//   // Collision
//   if (
//     birdY > 240 || birdY < 0 || 
//     (pipeX < 35 && pipeX + 20 > 25 &&
//      (birdY - 5 < gapY || birdY + 5 > gapY + gapSize))
//   ) {
//     tft.setRotation(3);
//     tft.fillScreen(TFT_RED);
//     tft.setRotation(1);
//     tft.fillScreen(TFT_RED);
//     tft.setCursor(65, 150);
//     tft.print("GAME OVER");
//     delay(2000);

//     // Reset game state
//     birdY = 60; pipeX = 240; velocity = 0;
//     score = 0;
//     scored = false;
//     tft.setRotation(3);
//     tft.fillScreen(TFT_BROWN);
//     tft.setRotation(1);
//     tft.fillScreen(TFT_CYAN);

//     // Redraw initial score
//     tft.setCursor(10, 10);
//     tft.setTextColor(TFT_BLACK, TFT_CYAN);
//     tft.print("Score: 0");

//     oldBirdY = birdY;
//     oldPipeX = pipeX;
//     return;
//   }

//   // Draw
//   drawBird();
//   drawPipe(pipeX, gapY);

//   oldBirdY = birdY;
//   oldPipeX = pipeX;

//   delay(30);
// }
