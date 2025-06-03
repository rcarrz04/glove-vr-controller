/**
 * Header file for GloveData class

 @author Ruben Carrazco
 @version May 30, 2025
 */

#pragma once
#ifndef GLOVE_DATA_H
#define GLOVE_DATA_H
#include <Arduino.h>

// Struct to store data from Controller Glove
struct __attribute__((packed)) GlovePacket {
  uint8_t header;
  uint16_t flex[5];
  float acc[3];
  float gyro[3];
  float temp;
};

class GloveData {
  public:

  GloveData(HardwareSerial& serial, bool setCalibration);

  void begin();
  void update(bool setCalibration);

  bool hasNewPacket();
  const GlovePacket& get();

  void calibrateFlexBias();
  void getFlexAdjusted(int16_t adjustedFlex[5]);
  bool isCalibrated();

  String getGesture(int16_t vals_FS[5], double acc[3], double roll);

  void getGyroCalibrated(double outGyro[3]);
  void getAccCalibrated(double outAcc[3]);



  private:

  GlovePacket packet;
  HardwareSerial& port;
  bool valid;

  bool calibrating;
  bool calibrated;
  uint64_t calibrationCount;
  uint64_t calibrationSum[5];
  
  // Biased flex sensor array after calibration data was collected
  uint16_t flexBias[5] = {1625, 661, 1234, 1367, 1445};

  bool gyroCalibrated;
  uint16_t gyroCalibrationCount;
  double gyroBiasSum[3];

  // Biased gyro sensor array after calibration data was collected
  double gyroBias[3] = {0.00, 0.05, 0.04};

  // optional ACC calibration 
  bool accCalibrated;
  uint16_t accCalibrationCount;
  double accBiasSum[3];
  double accBias[3] = {0, 0, 0};

};

#endif


