# Project Documentation: Alarm System Using Arduino

## 1. Introduction

### 1.1 Motivation for Choosing the Project
I decided to undertake this project with the desire to enhance the security of my home. Given the high costs of commercial alarm systems and their limitations in terms of customization, I considered that developing my own system was the best alternative for me as I already had most of the components.

### 1.2 Purpose and Necessity of the Project
The aim of this project is to develop an efficient, affordable, and easy-to-use alarm system that can be customized according to the specific needs of each residence. The necessity of the project is quite evident, as any residence needs some form of protection. This system is inexpensive to make and simple enough to be recreated by anyone. One of its advantages over a commercial alarm system is the high degree of customization offered by the Arduino platform. Everyone can create such an alarm system according to personal needs.

## 2. System Description

### 2.1 Hardware Components
The system is developed using the Arduino platform and includes the following components:
- Arduino Mega 2560 development board
- 16-key 4x4 keypad (OKY0272)
- 16x2 LCD display (1602A)
- PIR motion sensor (SR602)
- Buzzer
- B10k potentiometer
- A red LED
- Two 270 ohm resistors

### 2.2 Component Connection Scheme
Details on each component and how they should be connected to the Arduino:
- The 4x4 keypad allows the user to enter the code to activate or deactivate the alarm. It has 8 pins named R1, R2, R3, R4, C1, C3, C3, C4. Each controls a row or a column of the keypad. They must be connected to the Arduino on digital pins 22 - 29.
- The buzzer emits sounds when the system is activated, deactivated, or when the alarm is triggered. Its + pin should be connected to Digital pin 30 and the - pin to GND.
- The Red LED indicates the alert state or normal operation of the system. Its anode should be connected to Digital pin 31 using a 270 ohm resistor and the cathode to GND.
- The PIR motion sensor detects movement near the system and triggers the alarm. It has three pins: an anode connected to 5V, a cathode connected to GND, and the OUT pin connected to Digital pin 32.
- The B10K potentiometer is used to adjust the contrast of the LCD display. It has three pins: an anode connected to 5V, a cathode connected to GND, and one pin connected to the V0 pin of the LCD.
- The LCD displays the status of the system and instructions for the user. It has 16 pins but only 12 are used. They are connected to the Arduino as follows:
  - VSS, RW, K - to GND
  - VDD - to 5V
  - V0 - to the middle pin of the potentiometer
  - RS - to Digital pin 33
  - E - to Digital pin 34
  - D4, D5, D6, D7 - to Digital pins 35, 36, 36, 38
  - A - connected to one end of the 270 ohm resistor, the other end of the resistor connected to 5V on the Arduino

## 3. Software Implementation

### 3.1 Libraries and Variables
- Include the LiquidCrystal library for LCD control and Keypad for managing the keyboard.
- Define the pins for the LCD and keypad, as well as for the buzzer, LED, and PIR sensor.
- Global variables `inputCode` and `correctCode` store the user-entered code and the correct code needed to activate or deactivate the alarm.
- The variable `alarmActive` indicates whether the alarm is activated, and `alarmTriggered` if the alarm has been triggered.

### 3.2 Setup Function
- Initializes serial communication, the LCD, and configures the pins accordingly.
- Displays the initial state of the alarm ("Alarm: OFF") on the LCD screen.

### 3.3 HandleKeypad Function
- Checks if a key has been pressed.
- If the # key is pressed, checks the entered code.
- For digits, they are added to `inputCode` and displayed on the LCD.
- If more than 4 digits are attempted, an error sound is emitted and re-entry of the code is requested.

### 3.4 Code Verification: checkCode()
- Verifies if the user-entered code is correct.
- If the code is correct, the alarm is activated or deactivated, resetting the wrong attempt counter.
- In case of a wrong code, the attempt counter is increased and an error message is displayed.
- If a wrong code is entered three times in a row, the alarm is forcefully activated.

### 3.5 Activating and Deactivating the Alarm: toggleAlarm()
- Changes the state of the alarm and updates the LCD display.
- Upon activation, a 10-second sequence of beeps and LED blinks is triggered to indicate activation.

### 3.6 Motion Monitoring: checkMotion()
- Uses the PIR sensor to detect movement.
- If the movement persists, a 10-second sequence of beeps and blinks is triggered.
- If the correct code is not entered within those 10 seconds, the alarm is activated.

### 3.7 Triggering the Alarm: triggerAlarm()
- Starts a while loop where an alarm sound and blink occur every 200 milliseconds.
- The only way to exit the loop is by entering the correct code.

### 3.8 Auxiliary Functions
- `showError()` - Displays an error message on the LCD and resets the display for new code entry.
- `beep()` - Emits a sound using the buzzer for a certain duration and at a certain frequency.
- `resetLcdSecondRow()` - Clears the second row of the LCD to prepare for new code entry.

## 4. Implementation Tips

### 4.1 Hardware Component Checking
Before integrating the system completely, it is good to test each component individually to ensure that connections are correct and that they function properly. I will add some tests that I did beforehand to ensure that all connections are made properly and each component functions.

### 4.2 Problems and Tips Encountered in Software
One of the problems I encountered, which is very important, is program blocking when delays are used. I tried to use as few as possible and used the `millis()` function where necessary to perform multiple tasks in parallel without blocking the program. A good practice I followed was to thoroughly test each functionality when I implemented it, so if problems arose, I knew they were due to the last changes made and did not spend much time solving them.

## 5. Improvements, Conclusions, and Resources

### 5.1 Improvements
- One of the first improvements that can be made to this system is adding a feature that allows changing the activation/deactivation code using the keyboard. An idea I had was to have a secret code made up of letters on the keyboard that currently serve no purpose, and by typing this secret code, the code could be changed.
- Another improvement that can be added to the system is the inclusion of a wireless module (e.g., ESP) that allows remote control of the alarm and sending notifications to the mobile phone when the alarm is triggered.

### 5.2 Conclusions
By carrying out this project, I learned that the Arduino platform is very extensive and many interesting things can be done. I enjoyed realizing this project, considering that I already had some of the components and wanted a home alarm system. I will definitely improve this project in the future because I do not want it to remain just a prototype, but I want to put it into operation.

### 5.3 Resources
- [Project Code: Pastebin link](https://pastebin.com/mYzT9mv7)
- [Keypad Library: Arduino Reference](https://www.arduino.cc/reference/en/libraries/keypad/)
