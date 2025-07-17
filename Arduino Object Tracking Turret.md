# Arduino Object Tracking Turret 

An autonomous object-tracking turret built using Arduino and OpenCV. This system uses computer vision to detect and track a human hand in real time using a webcam. When an object is detected, the turret locks on with precise servo control, activates a laser, sounds an alert, and updates its status on an LCD.  

The turret switches back to idle scanning mode when the object leaves the field of view. The entire system is controlled by a Python script running OpenCV, sending data to the Arduino via Serial communication.

---

## Features
- Hand tracking with OpenCV (MediaPipe)  
- 2-axis turret movement via MG996R servos  
- Laser pointer + buzzer activation when hand is detected  
- LCD display showing system status (`Scanning area`, `Object detected!`, etc.)  
- Idle scanning mode with randomized pan/tilt angles  
- Real-time serial communication between Python and Arduino  
- Manual hardware build using DIY components + repurposed laser casing  

---

## Hardware Used
- Arduino Uno R3  
- 2x MG996R servo motors (pan & tilt)  
- Logitech C920 webcam *(or compatible webcam)*  
- Gravity: I2C LCD1602 Arduino LCD Display Module (Green)  
- DIY laser pointer (pen casing + 5V laser diode)  
- Passive buzzer  
- Breadboard + jumper wires  
- 3x 5V 2A power supplies (for servos and laser stability)

---

## How It Works
1. In idle mode, the turret randomly scans the environment using pan/tilt servos.  
2. When a hand is detected via webcam using OpenCV:
   - Turret locks onto the hand's position   
   - Displays `Object detected!` on the LCD  
   - Continues tracking until hand leaves frame  
3. When hand is no longer detected:
   - Turret returns to idle scan mode  
   - LCD updates to `Scanning area...`  

All vision and tracking logic is handled by Python, with servo control and outputs managed by Arduino.

---

##  Demo / Media

[ Watch the turret in action](https://youtube.com/YOUR_VIDEO_LINK_HERE)

---

##  Author

**Phong Ngo** – Mechatronics Engineering Student  
[LinkedIn](https://linkedin.com/in/YOUR-LINKEDIN-HERE)

---



