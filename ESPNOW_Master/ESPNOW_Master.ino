//MASTER

#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xDD, 0x57, 0x84};

const int btnUp = 13;
const int btnDown = 12;
const int btnLeft = 14;
const int btnRight = 27;
const int btnA = 26;
const int btnB = 25;

char lastSent = 'S';

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnA, INPUT_PULLUP);
  pinMode(btnB, INPUT_PULLUP);
}

void loop() {
  char cmd = 'S';

  if (digitalRead(btnUp) == LOW) cmd = 'F';
  else if (digitalRead(btnDown) == LOW) cmd = 'B';
  else if (digitalRead(btnLeft) == LOW) cmd = 'L';
  else if (digitalRead(btnRight) == LOW) cmd = 'R';
  else if (digitalRead(btnA) == LOW) cmd = 'X';
  else if (digitalRead(btnB) == LOW) cmd = 'Y';

  if (cmd != lastSent) {
    esp_now_send(broadcastAddress, (uint8_t *)&cmd, sizeof(cmd));
    lastSent = cmd;
    Serial.print("Sent: ");
    Serial.println(cmd);
  }

  delay(100); // debounce + reduce traffic
}
