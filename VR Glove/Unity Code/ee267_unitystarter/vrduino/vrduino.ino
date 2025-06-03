/**
 * EE267 Virtual Reality
 * Final Project
 * Orientation Tracking with IMUs Arduino Programming
 *
 * Instructor: Gordon Wetzstein <gordon.wetzstein@stanford.edu>
 *
 * The previous C++/OpenGL version was developed by Robert Konrad in 2016, and
 * the JavaScript/WebGL version was developed by Hayato Ikoma in 2017.
 *
 * The VRduino board is developed by Keenan Molner in 2017.
 * The current version of this Arduino codes are developed mainly by Marcus Pan
 * and Keenan Molner in 2017 and 2018.
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2020/04/01
 * This version uses Three.js (r115), stats.js (r17) and jQuery (3.2.1).
 */


#include <Wire.h>
#include "OrientationTracker.h"
#include "TestOrientation.h"
#include "GloveData.h"

// Implement and store glove data
const bool setCalibration = true;
GloveData glove(Serial1, setCalibration);

static Quaternion gloveQComp = Quaternion(1, 0, 0, 0);  // identity quaternion
static unsigned long prevGloveTime = 0;
static double gloveDeltaT = 0.01;
const double alphaGlove = 0.95;

//complementary filter value [0,1].
//1: ignore acc tilt, 0: use all acc tilt
double alphaImuFilter = 0.95;

//if true, get imu values from recorded data in external file
//if false, get imu values from live sampling.
bool simulateImu = false;

//if test is true, then run tests in TestOrientation.cpp and exit
bool test = false;

//if measureImuBias is true, measure imu bias and variance
bool measureImuBias = true;

//if measureBias is false, set the imu bias to the following:
double gyrBiasSet[3] = {0.17408, -0.19935, 0.44581};

//initialize orientation tracker
OrientationTracker tracker(alphaImuFilter, simulateImu);

//stream mode
//To change what the Teensy is printing out, set streamMode
//to one of the following values.
//You can change the streamMode in real time by sending the
//corresponding number to the Teensy through the Serial Monitor.

//bias values, read frequency
const int INFO   = 0;

//gyro values after bias subtraction
const int GYR    = 3;

//acc values
const int ACC    = 4;

//quaternion from comp filter
const int QC     = 5;

// IMU Data from Glove 
const int G_IMU = 1;

// Flex Sensor Data from Glove
const int G_FS = 2;

//chose which values you want to stream
int streamMode = QC;

//variables to measure read frequency
int nReads = 0;
unsigned long prevTime = 0;

//runs when the Teensy is powered on
void setup() {
  Serial.println("Teensy Initialized...");
  Serial.begin(115200);

  if (test) {
    delay(1000);
    testMain();
    return;

  }

  // HMD IMU INITIALIZATION

  tracker.initImu();

  if (measureImuBias) {

    Serial.printf("Measuring bias\n");
    tracker.measureImuBiasVariance();

  } else {

    tracker.setImuBias(gyrBiasSet);

  }
  prevTime = micros();

}

double getPitchFromQuaternion(const Quaternion& q) {
  // Pitch: rotation around X (forward/backward tilt)
  return atan2(
    2.0 * (q.q[0]*q.q[1] + q.q[2]*q.q[3]),
    1.0 - 2.0 * (q.q[1]*q.q[1] + q.q[2]*q.q[2])
  ) * RAD_TO_DEG;
}

double getRollFromQuaternion(const Quaternion& q) {
  // Roll: rotation around Z (left/right tilt in your custom frame)
  return asin(
    2.0 * (q.q[0]*q.q[2] - q.q[3]*q.q[1])
  ) * RAD_TO_DEG;
}

void loop() {

  // RETRIEVE GLOVE DATA
  glove.update(setCalibration);

  //reads in a single char to update behaviour. options:
  //0-5: set streamMode. See mapping above.
  //r  : reset orientation estimates to 0.
  //b  : remeasure bias
  if (Serial.available()) {

    int read = Serial.read();

    //check for streamMode
    int modeRead = read - 48;

    if (modeRead >= 0 && modeRead <= 5) {

      streamMode = modeRead;

    } else  if (read == 'r') {

      //reset orientation estimate to 0
      tracker.resetOrientation();

      // Reset orientation estimate for Glove
      gloveQComp = Quaternion(1, 0, 0, 0);  // identity quaternion
      Serial.println("Orientation reset.");


    } else if (read == 'b') {

      //measure imu bias
      Serial.printf("Measuring bias\n");
      tracker.measureImuBiasVariance();


    }
  }



  if (test) {
    return;
  }


  if (streamMode == INFO) {
    //print out number of reads / sec
    unsigned long now = micros();
    if ((now - prevTime) > 1000000) {
      Serial.printf("nReads/sec: %d\n", nReads);
      nReads = 0;
      prevTime = now;

      //print out bias/variance
      const double* gyrBias = tracker.getGyrBias();
      const double* gyrVariance = tracker.getGyrVariance();
      Serial.printf("GYR_BIAS: %.5f %.5f %.5f\n", gyrBias[0], gyrBias[1], gyrBias[2]);
      Serial.printf("GYR_VAR: %.5f %.5f %.5f\n", gyrVariance[0], gyrVariance[1], gyrVariance[2]);

      const double* accBias = tracker.getAccBias();
      const double* accVariance = tracker.getAccVariance();
      Serial.printf("ACC_BIAS: %.3f %.3f %.3f\n", accBias[0], accBias[1], accBias[2]);
      Serial.printf("ACC_VAR: %.3f %.3f %.3f\n", accVariance[0], accVariance[1], accVariance[2]);

    }
  }

  bool imuTrack = tracker.processImu();

  //return if there's no new values
  if (!imuTrack) {
    return;
  }

  nReads++;

  const double* acc = tracker.getAcc();
  const double* gyr = tracker.getGyr();
  const Quaternion& qComp = tracker.getQuaternionComp();

  

  if (streamMode == GYR) {

    //print out gyr values
    Serial.printf("GYR: %.3f %.3f %.3f",gyr[0], gyr[1], gyr[2]);

  } else if (streamMode == ACC) {

    //print out acc values
    Serial.printf("ACC: %.3f %.3f %.3f",
      acc[0], acc[1], acc[2]);

  } else if (streamMode == QC) {

    double gloveCurrentTime = micros() / 1000000.0;

    if (prevGloveTime == 0.0) {
      prevGloveTime = gloveCurrentTime;
    }
    
      // Always stream quaternion
      Serial.printf("QC %.3f %.3f %.3f %.3f",
        qComp.q[0], qComp.q[1], qComp.q[2], qComp.q[3]);

      static String lastValidGesture = "None";

      // Get latest flex + gesture
      double gyroCal[3];
      double accCal[3];
      glove.getGyroCalibrated(gyroCal);
      glove.getAccCalibrated(accCal);

      double acc_d[3] = { accCal[0], accCal[1], accCal[2] };
      double gyr_d[3] = { gyroCal[0], gyroCal[1], gyroCal[2] };


      gloveDeltaT = gloveCurrentTime - prevGloveTime;
      prevGloveTime = gloveCurrentTime;

      updateQuaternionComp(gloveQComp, gyr_d, acc_d, gloveDeltaT, alphaGlove);

      double roll = getRollFromQuaternion(gloveQComp);

      int16_t adjusted[5];
      glove.getFlexAdjusted(adjusted);
      String currentGesture = glove.getGesture(adjusted, accCal, roll);

      // Only update memory if it's a valid gesture
      if (currentGesture != "None") {
        lastValidGesture = currentGesture;
      }

      // Always send the remembered gesture
      Serial.printf(" GESTURE %s\n", lastValidGesture.c_str());


  } else if (streamMode == G_IMU && glove.hasNewPacket()) {
    

    // Streams pitch and roll data calculated from glove IMU

    double gloveCurrentTime = micros() / 1000000.0;

    if (prevGloveTime == 0.0) {
      prevGloveTime = gloveCurrentTime;
    }
    double gyroCal[3];
    double accCal[3];
    glove.getGyroCalibrated(gyroCal);
    glove.getAccCalibrated(accCal);

    // Ensures glove IMU is calibrated first
    if (glove.isCalibrated()) {
      double acc_d[3] = { accCal[0], accCal[1], accCal[2] };
      double gyr_d[3] = { gyroCal[0], gyroCal[1], gyroCal[2] };

      

      gloveDeltaT = gloveCurrentTime - prevGloveTime;
      prevGloveTime = gloveCurrentTime;

      updateQuaternionComp(gloveQComp, gyr_d, acc_d, gloveDeltaT, alphaGlove);

      // Extracts pitch and roll from quaternion produced from IMU data
      double pitch = getPitchFromQuaternion(gloveQComp);
      double roll = getRollFromQuaternion(gloveQComp);
      
      Serial.printf("%.2f %.2f\n", pitch, roll);
    }

  } else if (streamMode == G_FS && glove.hasNewPacket()) {
    // Streams flex sensor data 
    int16_t adjusted[5];
    glove.getFlexAdjusted(adjusted);
    Serial.printf("%d %d %d %d %d", adjusted[0], adjusted[1], adjusted[2], adjusted[3], adjusted[4]);
  }

  delay(10);

}
