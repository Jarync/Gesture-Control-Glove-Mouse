# Gesture Control Glove Mouse (Wearable HID)

![Hardware](https://img.shields.io/badge/Hardware-Arduino%20Leonardo%2FPro%20Micro-blue.svg)
![Sensors](https://img.shields.io/badge/Sensors-MPU6050%20%7C%20Flex%20Sensor-orange.svg)
![Feature](https://img.shields.io/badge/Feature-HID%20Mouse%20Emulation-green.svg)

## 📖 Introduction

This project is a wearable Human-Computer Interaction (HCI) device that converts hand gestures into computer mouse inputs. By integrating a gyroscope and a flex sensor, the system allows users to control the cursor by moving their hand in the air and scroll pages by bending their fingers and rotating their wrist.

It utilizes the **Arduino HID (Human Interface Device)** protocol, meaning it is plug-and-play and works on any computer without installing custom drivers.

### ✨ Key Functionalities
* **Cursor Movement:** Controlled by the **MPU6050 Gyroscope** (Hand rotation maps to X/Y movement).
* **Clicking:** Physical buttons for Left/Right clicks.
* **Smart Scrolling:**
    * **Activation:** Bend the finger (Flex Sensor triggers).
    * **Direction:** Rotate palm **UP** to scroll UP, rotate palm **DOWN** to scroll DOWN.
* **Signal Filtering:** Implemented a **Sliding Average Filter** to smooth out sensor noise and prevent jittery cursor movement.

---

## 🛠️ Hardware & Bill of Materials (BOM)

| Component | Type | Role |
| :--- | :--- | :--- |
| **Microcontroller** | Arduino Leonardo / Pro Micro | Supports Native USB HID (ATmega32U4) |
| **IMU Sensor** | MPU6050 (Gyro + Accel) | Detects hand rotation for cursor movement |
| **Flex Sensor** | Resistive Flex Sensor (2.2" or 4.5") | Detects finger bending for scrolling mode |
| **Buttons** | 2x Tactile Push Buttons | Left Click & Right Click |
| **Resistors** | 1x 10kΩ (Pull-down for Flex Sensor) | Voltage divider for analog reading |
| **Wiring** | Jumper Wires & Breadboard | Prototyping connections |

> **Note:** Standard Arduino Uno **cannot** serve as a native USB Mouse without firmware hacking. An ATmega32U4 based board (Leonardo/Micro) is required for the `Mouse.h` library.

---

## 🔌 Circuit Diagram

The following Fritzing diagram illustrates the connection between the MPU6050 (I2C), Flex Sensor (Analog Divider), and Buttons (Digital Pull-up).

![mouse_bb3](https://github.com/user-attachments/assets/b54aa7a5-c363-4b93-9dfc-721be46e2420)


### Pin Configuration
* **MPU6050:** VCC -> 5V, GND -> GND, SDA -> SDA (D2), SCL -> SCL (D3)
* **Flex Sensor:** A0 (connected via voltage divider with 10kΩ resistor)
* **Left Button:** Pin 6 (Internal Pull-up)
* **Right Button:** Pin 7 (Internal Pull-up)

---

## 💻 Logic & Algorithms

### 1. Cursor Control (Gyroscope)
The cursor movement is driven by the angular velocity ($g_x, g_y$) from the gyroscope.
$$MouseX = \frac{GyroX}{Sensitivity}, \quad MouseY = \frac{GyroY}{Sensitivity}$$
* A **deadzone threshold** is applied to ignore minor tremors.

### 2. Scroll Gesture (Sensor Fusion)
Scrolling is a compound gesture involving two sensors:
1.  **Trigger:** Flex sensor value > Threshold (Finger Bent).
2.  **Direction:** Accelerometer Z-axis ($a_z$) determines palm orientation.
    * $a_z > 0.5g$ (Palm Up) $\rightarrow$ **Scroll Up**
    * $a_z < -0.5g$ (Palm Down) $\rightarrow$ **Scroll Down**

---
## 📸 Prototype Gallery

### 1. Hardware Setup

<img width="527" height="240" alt="屏幕截图 2025-11-23 221910" src="https://github.com/user-attachments/assets/9a34b603-7c09-44e0-b278-4103cbb698da" />

---

## 🚀 How to Run

1.  **Hardware Setup:** Assemble the circuit as per the diagram. Ensure the Flex Sensor is mounted on a glove finger.
2.  **Libraries:** Install the `MPU6050` library via Arduino IDE.
3.  **Upload:** Flash the provided `.ino` code to an Arduino Leonardo or Pro Micro.
4.  **Usage:**
    * Connect the Arduino USB to a PC.
    * Move the board to move the cursor.
    * Press buttons to click.
    * Bend the sensor and flip your hand to scroll.

---

## ©️ Intellectual Property & License

**Copyright © 2025 Chen Junxu. All Rights Reserved.**

### ⚠️ Disclaimer (Prototype Use Only)
This repository contains **prototype firmware and schematics** developed for educational and demonstration purposes. 
* It **does NOT** represent the final mass-produced commercial product of any associated company or client. 
* Specific proprietary parameters, industrial safety protocols, and commercial PCB designs have been **excluded** to protect intellectual property.

### 📜 Usage Policy (CC BY-NC 4.0)
This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International License**.

1.  **Non-Commercial Use:** You are free to use, modify, and study this code for **personal learning, academic research, or hobbyist projects**.
2.  **No Commercial Deployment:** You are **strictly prohibited** from using this code or design for commercial products, paid services, or industrial deployment without written permission.
3.  **Attribution:** If you use this project in your work or portfolio, you must credit the author (**Chen Junxu**) and link back to this repository.

---
