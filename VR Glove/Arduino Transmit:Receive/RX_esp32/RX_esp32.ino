#define RECEIVER_MODE

#ifdef RECEIVER_MODE

#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"

struct __attribute__((packed)) GlovePacket {
  uint8_t header;
  uint16_t flex[5];
  float acc[3];
  float gyro[3];
  float temp;
};

GlovePacket latestPacket;

// Receive packet of data from transmitting ESP32
void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  Serial.println(len);
  if (len == sizeof(GlovePacket)) {
    memcpy(&latestPacket, data, len);

    if (latestPacket.header == 0xAA) {
      Serial.println("Received valid packet via ESP-NOW");
      for (int i = 0; i < 5; i++) {
        Serial.printf("Flex[%d]: %d\n", i, latestPacket.flex[i]);
      }
      Serial.printf("AccX: %.2f | Temp: %.2f\n", latestPacket.acc[0], latestPacket.temp);

      Serial2.write((uint8_t*)&latestPacket, sizeof(GlovePacket));
    }
  } else {
    Serial.printf("Invalid packet size: %d bytes\n", len);
  }
}


void setup() {
  Serial.begin(9600);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // TX to Teensy RX2
  delay(3000);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ESP-NOW ready");
}

void loop() {
}

#endif
