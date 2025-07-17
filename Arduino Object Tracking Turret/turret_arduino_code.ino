#include <Servo.h>  // Library to control servo motors
#include "DFRobot_RGBLCD1602.h"  // Library to control the RGB 1602 LCD display

// Create servo objects for pan (left/right) and tilt (up/down) movement
Servo panServo;
Servo tiltServo;

// Create an LCD object. 
// 0x6B is the I2C address for the RGB LCD module (V1.1), 16 columns x 2 rows
DFRobot_RGBLCD1602 lcd(/*RGBAddr*/ 0x6B, /*lcdCols*/ 16, /*lcdRows*/ 2);

// Track when the last servo movement occurred (in milliseconds)
unsigned long lastMoveTime = 0;

// Track when the last dot animation was updated on the LCD
unsigned long lastDotUpdate = 0;

// Flag to indicate if an object was recently detected
bool objectDetected = false;

// Store the last text shown on the LCD to prevent unnecessary updates
String lastDisplay = "";

// Keep track of how many dots are shown in the scanning animation
int dotCount = 1;

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud (must match Python side)
  
  // Attach servos to digital pins 9 and 10
  panServo.attach(9);
  panServo.write(180);
  //Initial servo angles
  tiltServo.attach(10);
  tiltServo.write(180);

  // Initialise the LCD
  lcd.init();
  lcd.setCursor(0, 0);       // Set cursor to top-left corner
  lcd.print("System Ready"); // Display startup message
}

void loop() {
  // Check if there's incoming serial data from the PC (e.g. from Python/OpenCV)
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read full line from serial (until newline)
    input.trim();  // Remove leading/trailing whitespace or newline chars

    // If the incoming message is "DETECTED", show object detected state
    if (input == "DETECTED") {
      objectDetected = true;

      // Only update LCD if it's not already displaying this message
      if (lastDisplay != "DETECTED") {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Object detected!");
        lastDisplay = "DETECTED";
      }
    } 
    
    else {
      // Assume it's pan/tilt angle data in the format: "123,88", finds the location of the comma to split the string cleanly into pan and tilt
      int commaIndex = input.indexOf(',');
      
      // Only process if a comma was found
      if (commaIndex > 0) {
        // Extract pan and tilt values
        int pan = input.substring(0, commaIndex).toInt();
        int tilt = input.substring(commaIndex + 1).toInt();

        // Move the servos to the received angles, constrained between 0–180 degrees
        panServo.write(constrain(pan, 0, 180));
        tiltServo.write(constrain(tilt, 0, 180));

        // Record the time of this movement
        lastMoveTime = millis();

        // Reset detection flag so scanning can resume after timeout
        objectDetected = false;
      }
    }
  }

  // If no object detected and it's been more than 500ms since last servo movement...
  if (!objectDetected && (millis() - lastMoveTime > 500)) {

    // Update the scanning dots every 1 second
    if (millis() - lastDotUpdate > 1000) {
      lastDotUpdate = millis();  // Reset dot update timer

      // Increase dot count (cycles from 1 → 2 → 3 → 1...)
      dotCount++;
      if (dotCount > 3) dotCount = 1;

      // Generate the dots string based on count
      String dots = "";
      for (int i = 0; i < dotCount; i++) {
        dots += ".";
      }

      // Update LCD with "Scanning area..." animation
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scanning area" + dots);
      lastDisplay = "SCANNING";
    }
  }
}
