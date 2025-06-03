# Glove-Based VR Controller

This project implements a wearable glove interface for controller-free interaction in virtual reality. The glove uses five flex sensors and an IMU to classify hand gestures in real-time and track hand orientation. It communicates wirelessly with a VR headset via ESP-NOW, enabling intuitive gaze-and-gesture control of Unity-based virtual applications.

## Features

- Real-time gesture recognition (Open, Select, SwipeR)
- Quaternion-based HMD orientation tracking using IMU
- Wireless ESP-NOW communication between glove and headset
- Unity integration for interacting with a photo gallery, target game, and weather panel
- No cameras, no hand-held controllers

## Hardware Components

- ESP32 (x2)
- Adafruit Short Flex Sensors (x5)
- MPU6050 IMU
- Teensy 4.0
- 9V Battery + Linear Regulator
- Glove base (fabric, velcro)

## How to Download

1. **Clone the repository:**
   ```bash
   git clone https://github.com/rcarrz04/glove-vr-controller.git
