#include <WiFi.h>
#include <esp_now.h>

// Callback when data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  char c = (char)*data;
  Serial.print("Received: ");
  Serial.println(c);
  switch (c) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': left(); break;
    case 'R': right(); break;
    case 'X': action1(); break;
    case 'Y': action2(); break;
    case 'S': stop(); break;
    default: Serial.println("Unknown command."); break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {
}

// Action functions
void forward() {
  Serial.println("Function: forward");
}

void backward() {
  Serial.println("Function: backward");
}

void left() {
  Serial.println("Function: left");
}

void right() {
  Serial.println("Function: right");
}

void stop() {
  Serial.println("Function: stop");
}

void action1() {
  Serial.println("Function: action1 (X)");
}

void action2() {
  Serial.println("Function: action2 (Y)");
}
