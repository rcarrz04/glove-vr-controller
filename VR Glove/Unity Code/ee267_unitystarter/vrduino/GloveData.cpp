#include "GloveData.h"

const int SAMPLE_SIZE = 2000;
const int GYRO_CALIBRATION_SAMPLES = 5000;
const int ACC_CALIBRATION_SAMPLES = 5000;
const int THRESHOLD = 500;

GloveData::GloveData(HardwareSerial& serial, bool setCalibration)
  : port(serial), valid(false),
    calibrating(true), calibrated(false), calibrationCount(0),
    gyroCalibrated(false), gyroCalibrationCount(0), accCalibrationCount(0) {
    
  port.begin(115200);

  // Iniitalize variables
  if (!setCalibration) {
      for (int i = 0; i < 5; i++) {
      calibrationSum[i] = 0;
      flexBias[i] = 0;
    }
    for (int i = 0; i < 3; i++) {
      gyroBiasSum[i] = 0;
      gyroBias[i] = 0;
    }

    for (int i = 0; i < 3; i++) {
      accBiasSum[i] = 0;
      accBias[i] = 0;
    }

  }

}

void GloveData::begin() {
  port.begin(115200);
}

void GloveData::update(bool setCalibration) {
  static uint8_t buffer[sizeof(GlovePacket)];
  static size_t index = 0;
  static bool syncing = true;

  valid = false;

  while (port.available()) {
    uint8_t byte = port.read();

    if (syncing) {
      if (byte == 0xAA) {
        buffer[0] = byte;
        index = 1;
        syncing = false;
      }
    } else {
      buffer[index++] = byte;

      if (index == sizeof(GlovePacket)) {
        memcpy(&packet, buffer, sizeof(GlovePacket));

        if (packet.header == 0xAA) {
          valid = true;

          if (!setCalibration){
            // Flex calibration
            if (calibrating && calibrationCount < SAMPLE_SIZE) {
              for (int i = 0; i < 5; i++) {
                calibrationSum[i] += packet.flex[i];
              }
              calibrationCount++;

              if (calibrationCount == SAMPLE_SIZE) {
                for (int i = 0; i < 5; i++) {
                  flexBias[i] = calibrationSum[i] / SAMPLE_SIZE;
                }
                calibrated = true;
                calibrating = false;
                Serial.println("Flex calibration complete");
                Serial.println(flexBias[0]);
                Serial.println(flexBias[1]);
                Serial.println(flexBias[2]);
                Serial.println(flexBias[3]);
                Serial.println(flexBias[4]);
              }
            }
            
            Serial.print(gyroCalibrationCount);
            // Gyro calibration
            if (!gyroCalibrated && gyroCalibrationCount < GYRO_CALIBRATION_SAMPLES) {
              for (int i = 0; i < 3; i++) {
                if (!isnan(packet.gyro[i]) && !isinf(packet.gyro[i])) {
                  gyroBiasSum[i] += packet.gyro[i];
                }
              }
              gyroCalibrationCount++;

              if (gyroCalibrationCount == GYRO_CALIBRATION_SAMPLES) {
                for (int i = 0; i < 3; i++) {
                  if (GYRO_CALIBRATION_SAMPLES > 0)
                    gyroBias[i] = gyroBiasSum[i] / GYRO_CALIBRATION_SAMPLES;
                  else
                    gyroBias[i] = 0;
                }
                gyroCalibrated = true;
                Serial.println("Gyro calibration complete");
                Serial.println(gyroBias[0]);
                Serial.println(gyroBias[1]);
                Serial.println(gyroBias[2]);
              }

            }
            // Acc calibration
            if (!accCalibrated && accCalibrationCount < ACC_CALIBRATION_SAMPLES) {
              for (int i = 0; i < 3; i++) {
                if (!isnan(packet.acc[i]) && !isinf(packet.acc[i])) {
                  accBiasSum[i] += packet.acc[i];
                }
              }
              accCalibrationCount++;

              if (accCalibrationCount == ACC_CALIBRATION_SAMPLES) {
                for (int i = 0; i < 3; i++) {
                  if (ACC_CALIBRATION_SAMPLES > 0)
                    accBias[i] = accBiasSum[i] / ACC_CALIBRATION_SAMPLES;
                  else
                    accBias[i] = 0;
                }
                accCalibrated = true;
                Serial.println("Acc calibration complete");
                Serial.println(accBias[0]);
                Serial.println(accBias[1]);
                Serial.println(accBias[2]);
              }

            }

          } else {
            // Done calibrating
            calibrated = true;
            calibrating = false;
            gyroCalibrated = true;
            accCalibrated = true;
          }
        }

        // Reset for next packet
        index = 0;
        syncing = true;
      }
    }
  }
}

bool GloveData::hasNewPacket() {
  return valid;
}

const GlovePacket& GloveData::get() {
  return packet;
}

// Calibrate sensor information 
void GloveData::getFlexAdjusted(int16_t adjustedFlex[5]) {
  for (int i = 0; i < 5; i++) {
    adjustedFlex[i] = (int16_t)packet.flex[i] - (int16_t)flexBias[i];

  }
}

void GloveData::getGyroCalibrated(double outGyro[3]) {
  for (int i = 0; i < 3; i++) {
    outGyro[i] = packet.gyro[i] - (float)gyroBias[i];

  }
}

void GloveData::getAccCalibrated(double outAcc[3]) {
  for (int i = 0; i < 3; i++) {
    outAcc[i] = packet.acc[i];
  }
}

bool GloveData::isCalibrated() {
  return calibrated && gyroCalibrated && accCalibrated;
}

void GloveData::calibrateFlexBias() {
  calibrated = false;
  calibrating = true;
  calibrationCount = 0;
  for (int i = 0; i < 5; i++) {
    calibrationSum[i] = 0;
  }
}

// Extract gesture based on data reading
String GloveData::getGesture(int16_t vals_FS[5], double acc[3], double roll) {
  if (!valid) {
    return "None";
  }

  // closed hand gesture
  if (vals_FS[0] > THRESHOLD && vals_FS[2] > THRESHOLD  && vals_FS[3] > THRESHOLD && vals_FS[4] > THRESHOLD ) {
    return "Select";

  } else if (vals_FS[0] < THRESHOLD && vals_FS[2] > THRESHOLD  && vals_FS[3] > THRESHOLD && vals_FS[4] > THRESHOLD) {
      // Hand must be upright in index position and a fast lateral movement must be detected to swipe
      if (acc[0] < -7.0) {
        if (acc[2] < -5.0 || acc[2] > 5.0) {
          return "SwipeR";
        }
      }
  } 

  // The gesture below is implemented but unused (detects if palm is up or down)
  if (roll > 20.0) {
    return "pup";
  } else if (roll < -20.0) {
    return "pdown";
  }

  // Returns open by default
  return "Open";
}
