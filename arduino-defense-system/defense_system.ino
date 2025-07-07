
            #include <Servo.h>
#include <LiquidCrystal_I2C.h>

//Password stuff
String correctPassword = "0606"; //The desired password
String  passwordInput = ""; //To store user input for comparison with the actual password

//For Ultrasonic sensor 
int trig_Pin = 8;
int echo_Pin = 12;
int thresholdDistance = 15;
bool objectDetected = false;
unsigned long duration;
unsigned long distance;

//For joystick 
int joyX = A0;
int joyY = A1;
int joystickButton = 2;
bool manualMode = false;
unsigned long jTime;
unsigned long debounceDelay = 200;
int buttonPressCount = 0;


//For LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long previousMillis = 0;
unsigned long blinkInterval = 500;
bool islcdOn = false;
unsigned long lTime;
unsigned long dTime;
unsigned long displayInterval = 1000;
int dots = 1;


//For buzzer
int buzzer_Pin = 4;

//For RGB LEDs
int redled_Pin = 3;
int greenled_Pin = 5;
int blueled_Pin = 6;

//For servo motor #Horizontal
int servoH_Pin = 9;
Servo myServoH;
int servoPosH = 0;

//For servo motor #Vertical
int servoV_Pin = 10;
Servo myServoV;
int servoPosV = 0;

//For laser
int laser_Pin = 7;

//Servo times
unsigned long moveInterval = 5000;
unsigned long sTime;

void radarFunction(){
  Serial.print(servoPosH);
  Serial.print(",");
  Serial.println(distance);
}

void showMenu(){ //Prints the menu options to the serial monitor, once password is correct
  Serial.println("Menu:");
  Serial.println("1. Change thresholdDistanceForSensor");
  Serial.println("2. Change moveIntervalForServo");
  Serial.println("3. Change displayIntervalForLCD");
  Serial.println("Type \"exit\" to leave the menu. ");

   while (true){ //creates an infinite loop until a break in code, loop keeps menu active
    if (Serial.available()){ //Checks if there is any input available, any data to read from serial
      String passwordInput = Serial.readString(); //Reads string from serial
       passwordInput.trim(); //Cleans up extra spaces or new lines, makes it much easier for comparison

      if (passwordInput.equalsIgnoreCase("exit")){ //Exit the menu if user types exit, that case is used to compare two strings without considering case differences, treats all forms of "exit" same
        Serial.println("Exiting menu...");
        return;
      }

      int choice =  passwordInput.toInt(); //Converts user input string into an integer using toInt
      if (choice >= 1 && choice <= 9){ //Checks if the choice is between 1-9, if not goes to else case

      switch(choice){ //akin to switch statment, each case has its own function running
        case 1:
        thresholdDistanceForSensor();
        break;

        case 2:
        moveIntervalForServo();
        break;

        case 3:
        displayIntervalForLcd();
        break;
      }
      }

      else{
        Serial.println("Done. Type \"exit\" to leave menu.");
      }
    }
  }
}

void thresholdDistanceForSensor(){
  Serial.println("Enter a new thresholdDistanceForSensor value in millisseconds:");
 
  while (Serial.available() == 0){ //loop will continue to run till data is available
    //Wait for user input
  }

  unsigned long newValue = Serial.parseInt(); //Reads digits from serial and converts them into an int value, skips any white spaces(breaks down into comps)
  thresholdDistance = newValue; //Updates to new value
  Serial.print("thresholdDistanceForSensor updated to: ");
  Serial.println(thresholdDistance);
}

void moveIntervalForServo(){
  Serial.println("Enter a new moveIntervalForServo value in millisseconds:");
 
  while (Serial.available() == 0){
  }

  unsigned long newValue = Serial.parseInt();
  moveInterval = newValue;
  Serial.print("moveIntervalForServo updated to: ");
  Serial.println(moveInterval);
}

void displayIntervalForLcd(){
  Serial.println("Enter a new displayIntervalForLcd value in millisseconds:");
 
  while (Serial.available() == 0){
  }

  unsigned long newValue = Serial.parseInt();
  displayInterval = newValue;
  Serial.print("displayIntervalForLcd updated to: ");
  Serial.println(displayInterval);
}





void lcdBlink() {
  if (millis() - lTime >= blinkInterval){
    lTime = millis();
    islcdOn = !islcdOn;
  }
  if (islcdOn) {
    lcd.setCursor(0, 0);
    lcd.print("Warning!!!");
    lcd.setCursor(0, 1);
    lcd.print("Object detected");
  }
  else {
    lcd.clear();
  }
 }

 void scanningDisplay() {
  if (millis() - dTime >= displayInterval) {
    dTime = millis();

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Scanning area");

    for (int i = 0; i < dots; ++i) {
      lcd.print(".");
    }

    dots++;
    if (dots > 3) {
      dots = 1;
    }
  }
 }

 void checkJoystick() {
  if (digitalRead(joystickButton) == LOW) {
    manualMode = !manualMode;

    if (millis() - jTime >= debounceDelay) {
      buttonPressCount++;
      jTime = millis();

      if (buttonPressCount == 2) {
        automatedMode();
      }
    }
  }
 }
      
    
  

  
 
 
 

 void objectDetection() {
  //Trigger trig pin
  digitalWrite(trig_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_Pin, LOW);

  //Trigger echo pin
  duration = pulseIn(echo_Pin, HIGH);
  distance = (duration * 0.0343) / 2; //Convert to cm

  //Check if object is within threshold distance or not
  if (distance > 0 && distance < thresholdDistance){
    objectDetected = true;
  }
  else{
    objectDetected = false;
  }
 }

 void triggerAlert() {
  digitalWrite(laser_Pin, HIGH);
  tone(buzzer_Pin, 1000);
  lcdBlink();
  digitalWrite(redled_Pin, HIGH);
  digitalWrite(greenled_Pin, LOW);
 }

 void clearAlert() {
  noTone(buzzer_Pin);
  digitalWrite(laser_Pin, LOW);
  digitalWrite(redled_Pin, LOW);
  digitalWrite(greenled_Pin, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Area Clear");
  lcd.setCursor(0, 1);
  scanningDisplay();
 }

 void automatedMode() {
  objectDetection();

  if (objectDetected){
    triggerAlert();
  }
  else{
    clearAlert();

    if (millis() - sTime >= moveInterval) {
      sTime = millis();

      servoPosH = random(0, 180);
      servoPosV = random(0, 90);

      myServoH.write(servoPosH);
      myServoV.write(servoPosV);
    }
  }
 }

 void joystickMode() {
  objectDetection();

  if (objectDetected) {
    triggerAlert();
  }
  else {
  clearAlert();
  

  int joystickX = analogRead(joyX);
  int joystickY = analogRead(joyY);

  servoPosH = map(joystickY, 0, 1023, 0, 180);
  servoPosV = map(joystickX, 0, 1023, 0, 90);

  myServoH.write(servoPosH);
  myServoV.write(servoPosV);
  }
 }
 
void setup() {
 pinMode(redled_Pin, OUTPUT);
 pinMode(greenled_Pin, OUTPUT);
 pinMode(blueled_Pin, OUTPUT);
 pinMode(trig_Pin, OUTPUT);
 pinMode(echo_Pin, INPUT);
 pinMode(buzzer_Pin, OUTPUT);
 pinMode(laser_Pin, OUTPUT);
 pinMode(joystickButton, INPUT_PULLUP);

 //Setting the horizontal servo
 myServoH.attach(servoH_Pin); //Attaches servo to a specific pin, to control servo
 myServoH.write(90);


 //Setting the vertical servo
 myServoV.attach(servoV_Pin); //Attaches servo to a specific pin, to control servo
 myServoV.write(90);


 //LCD stuff
 lcd.init();
 lcd.backlight();

 //Serial stuff
 Serial.begin(9600);
}


  
 void loop() {
   //Check for password input
  if (Serial.available()){
     String passwordInput = Serial.readString();
     passwordInput.trim(); //Removes any whitespace

    if ( passwordInput == correctPassword){
      Serial.println("Access Granted!");
      showMenu(); //Calls function after correct password
    }
    else{
      Serial.println("Wrong Password. Please try again:");
    }
  }
  checkJoystick();
  radarFunction();

  if(manualMode){
    joystickMode();
  }
  else{
    automatedMode();
  }
 }

          
