// Test file to verify Teensy communicaiton works


struct __attribute__((packed)) GlovePacket {
  uint8_t header;
  uint16_t flex[5];
  float acc[3];
  float gyro[3];
  float temp;
};

GlovePacket packet;

void setup() {
  Serial.begin(115200);      
  Serial1.begin(115200);    
  Serial.println("Teensy ready");
}

void loop() {
  // Only read if full packet is available
  if (Serial1.available() >= (int)sizeof(GlovePacket)) {
    Serial1.readBytes((uint8_t*)&packet, sizeof(GlovePacket));

    // check header for safe measure
    if (packet.header == 0xAA) {
      // Plot values for Serial Plotter
      Serial.print(packet.flex[0]); Serial.print('\t');
      Serial.print(packet.flex[1]); Serial.print('\t');
      Serial.print(packet.flex[2]); Serial.print('\t');
      Serial.print(packet.flex[3]); Serial.print('\t');
      Serial.print(packet.flex[4]); Serial.print('\t');
      Serial.print(packet.acc[0]);  Serial.print('\t');
      Serial.print(packet.gyro[2]); Serial.print('\t');
      Serial.println(packet.temp);
    }
  }
}
