#define GLOVE_MODE

#ifdef GLOVE_MODE

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "esp_wifi.h"


Adafruit_MPU6050 mpu;

// MAC address of receiver ESP32
uint8_t receiverMAC[] = { 0x3C, 0x8A, 0x1F, 0x5C, 0xDD, 0x40 };

const int flexPins[5] = {34, 32, 35, 39, 36};

// struct for sending data packet
struct __attribute__((packed)) GlovePacket {
  uint8_t header = 0xAA;
  uint16_t flex[5];
  float acc[3];
  float gyro[3];
  float temp;
};

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
  esp_wifi_set_promiscuous(false);

  delay(3000);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  if (!mpu.begin()) {
    Serial.println("Failed to init MPU6050!");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  Serial.println("Glove ESP-NOW sender initialized");
}

void loop() {
  GlovePacket packet;
  for (int i = 0; i < 5; i++) {
    packet.flex[i] = analogRead(flexPins[i]);
  }

  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  packet.acc[0] = a.acceleration.x;
  packet.acc[1] = a.acceleration.y;
  packet.acc[2] = a.acceleration.z;

  packet.gyro[0] = g.gyro.x;
  packet.gyro[1] = g.gyro.y;
  packet.gyro[2] = g.gyro.z;
  packet.temp = t.temperature;

  Serial.println(packet.flex[0]);
  Serial.println(packet.flex[1]);
  Serial.println(packet.flex[2]);
  Serial.println(packet.flex[3]);
  Serial.println(packet.flex[4]);
  
  // send packet 
  Serial.printf("Sending %d bytes via ESP-NOW\n", sizeof(packet));
  esp_now_send(receiverMAC, (uint8_t*)&packet, sizeof(packet));

  delay(10);  // ~100Hz
}

#endif
