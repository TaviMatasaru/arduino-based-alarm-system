#include <LiquidCrystal.h>
#include <Keypad.h>

// Definitions for LCD
const int rs = 33, en = 34, d4 = 35, d5 = 36, d6 = 37, d7 = 38;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Definitions for the keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //pins from Arduino to row pins
byte colPins[COLS] = {26, 27, 28, 29}; //pins from Arduino to column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin definitions for other components
const int buzzerPin = 30;
const int ledPin = 31;
const int pirPin = 32;

// Variables for managing the access code and the state of the alarm
String inputCode = ""; // Code entered by the user
String correctCode = "2017"; // Correct code for activating/deactivating the alarm
bool alarmActive = false; // Alarm state ON/OFF
bool alarmTriggered = false; // check if the alarm is triggered
int attemptCounter = 0; // Counter for the number of wrong attempts

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  lcd.setCursor(0, 0);
  lcd.print("Alarm: OFF");
  resetLcdSecondRow();
}

void loop() {
  handleKeypad();
  if (alarmActive && !alarmTriggered) {
    checkMotion();
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    beep(30, 500); // Emit a short sound when any key is pressed
    if (key == '#') { // Key to validate the entered code
      checkCode();
    } else if (isDigit(key)) {
      if (inputCode.length() < 4) { // Check the length of the entered code
        inputCode += key;
        lcd.setCursor(6 + inputCode.length(), 1); // Move the cursor on LCD
        lcd.print(key);
      } else { // A code longer than 4 digits will display an error and will reset
        beep(50, 200);
        showError("Type 4 digits!");
        inputCode = "";
      }
    } // Ignore other pressed keys
  }
}

void checkCode() {
  if (inputCode == correctCode) {
    beep(200, 1000); // Confirmation sound
    resetLcdSecondRow();
    attemptCounter = 0; // Reset wrong attempt counter
    alarmTriggered = false;
    toggleAlarm(); // Activate or deactivate the alarm
  } else {
    attemptCounter++;
    beep(50, 200);
    showError("Wrong Code!");
    if (attemptCounter >= 3 && !alarmTriggered) { // Trigger the alarm after 3 wrong codes
      alarmActive = true;
      triggerAlarm();
    }
  }
  inputCode = ""; // Reset the entered code
  resetLcdSecondRow();
}

void toggleAlarm() {
  alarmActive = !alarmActive;
  lcd.setCursor(0, 0);
  lcd.print(alarmActive ? "Alarm: ON       " : "Alarm: OFF      ");
  if (alarmActive) {
    // Blink LED and beep for 10 seconds
    for (int i = 0; i < 10; i++) {
      digitalWrite(ledPin, HIGH);
      beep(50, 600); // Sound for 1 second
      digitalWrite(ledPin, LOW);
      delay(1000);
    }
  }
}

void checkMotion() {
  if (digitalRead(pirPin) == HIGH) {
    // If motion is detected
    digitalWrite(ledPin, HIGH); // Turn on the LED
    beep(50, 800);
    delay(2000); // Wait 2 seconds to check if the motion persists
    if (digitalRead(pirPin) == HIGH) {
      // If the motion is still detected
      long startDeactivationTime = millis();
      while(millis() - startDeactivationTime <= 10000 && alarmActive){ // For 10 seconds
        handleKeypad();
        if(millis() % 1000 == 0){ // Blink and beep once a second
          digitalWrite(ledPin, HIGH);
          beep(50, 600);
          digitalWrite(ledPin, LOW);
        }
      }
      if(millis() - startDeactivationTime > 10000 && alarmActive)
        // If after 10 seconds the correct code has not been entered, trigger the alarm
        triggerAlarm();
    }
    digitalWrite(ledPin, LOW); // If the motion did not persist, turn off the LED
  }
}

void triggerAlarm() {
  alarmTriggered = true;
  lcd.setCursor(0, 0);
  lcd.print("Alarm: Triggered");
  while(alarmTriggered){ // While the alarm is triggered
    handleKeypad();
    if(millis() % 200 == 0){
      // Alarm sound and blink every 200 milliseconds
      digitalWrite(ledPin, HIGH);
      beep(100, 1500);
      digitalWrite(ledPin, LOW);
    }
  }
}

void showError(String message) {
  if(!alarmTriggered){
    long errorTimer = millis();
    while(millis() - errorTimer <= 2000){
      // Display the error message for 2 seconds
      lcd.setCursor(0, 1);
      lcd.print(message);
    }
  }
  resetLcdSecondRow();
}

void beep(unsigned int duration, unsigned int frequency) {
  // Emit a sound of a certain frequency for a certain duration
  tone(buzzerPin, frequency);
  delay(duration);
  noTone(buzzerPin);
}

void resetLcdSecondRow(){
  // Reset the second row of the LCD after a wrong code or an error
  lcd.setCursor(0, 1);
  lcd.print("Code:         ");
}
