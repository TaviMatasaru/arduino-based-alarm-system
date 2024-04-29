#include <LiquidCrystal.h>
#include <Keypad.h>

// Definițiile pentru LCD
const int rs = 33, en = 34, d4 = 35, d5 = 36, d6 = 37, d7 = 38;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Definițiile pentru tastatură
const byte ROWS = 4; //patru rânduri
const byte COLS = 4; //patru coloane
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //pinii de la Arduino la pinii randurilor
byte colPins[COLS] = {26, 27, 28, 29}; //pinii de la Arduino la pinii coloanelor
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Definițiile pinilor pentru celelalte componente
const int buzzerPin = 30;
const int ledPin = 31;
const int pirPin = 32;

// Variabile pentru gestionarea codului de acces și a stării alarmei
String inputCode = ""; // Codul introdus de utilizator
String correctCode = "2017"; // Codul corect pentru activarea/dezactivarea alarmei
bool alarmActive = false; // Starea alarmei ON/OFF
bool alarmTriggered = false; //verificarea daca alarma este declansată
int attemptCounter = 0; // Contor pentru numărul de încercări greșite

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
    beep(30, 500); // Emite un sunet scurt la apăsarea oricărei taste
    if (key == '#') { // Tasta pentru validarea codului tastat
      checkCode();
    } else if (isDigit(key)) {
      if (inputCode.length() < 4) { // Verificarea lungimii codului tastat
        inputCode += key;
        lcd.setCursor(6 + inputCode.length(), 1); // Mută cursorul pe LCD
        lcd.print(key);
      } else { // Un cod mai lung de 4 cifre va afișa o eroare și se va reseta
        beep(50, 200);
        showError("Type 4 digits!");
        inputCode = "";
      }
    } // Ignoră alte taste apăsate
  }
}

void checkCode() {
  if (inputCode == correctCode) {
    beep(200, 1000); // Sunet de confirmare
    resetLcdSecondRow();
    attemptCounter = 0; // Resetare contor încercări greșite
    alarmTriggered = false;
    toggleAlarm(); // Activează sau dezactivează alarma
  } else {
    attemptCounter++;
    beep(50, 200);
    showError("Wrong Code!");
    if (attemptCounter >= 3 && !alarmTriggered) { // Declanșează alarma după 3 coduri greșite
      alarmActive = true;
      triggerAlarm();
    }
  }
  inputCode = ""; // Resetează codul introdus
  resetLcdSecondRow();
}


void toggleAlarm() {
  alarmActive = !alarmActive;
  lcd.setCursor(0, 0);
  lcd.print(alarmActive ? "Alarm: ON       " : "Alarm: OFF      ");
  if (alarmActive) {
    // Blink LED și beep pentru 10 secunde
    for (int i = 0; i < 10; i++) {
      digitalWrite(ledPin, HIGH);
      beep(50, 600); // Sunet de 1 secundă
      digitalWrite(ledPin, LOW);
      delay(1000);
    }
  }
}

void checkMotion() {
  if (digitalRead(pirPin) == HIGH) {
    // Dacă detectează mișcare
    digitalWrite(ledPin, HIGH); // Aprinde LED-ul
    beep(50, 800);
    delay(2000); // Așteaptă 2 secunde pentru a verifica dacă mișcarea persistă
    if (digitalRead(pirPin) == HIGH) {
      // Dacă mișcarea încă este detectată
      long startDeactivationTime = millis();
      while(millis() - startDeactivationTime <= 10000 && alarmActive){ // Timp de 10 secunde
        handleKeypad();
        if(millis() % 1000 == 0){ // Blink si beep o data la o secundă
          digitalWrite(ledPin, HIGH);
          beep(50, 600);
          digitalWrite(ledPin, LOW);
        }
      }
      if(millis() - startDeactivationTime > 10000 && alarmActive)
        // Dacă după cele 10 secunde nu a fost introdus codul corect, declanșeză alarma
        triggerAlarm();
    }
    digitalWrite(ledPin, LOW); // Dacă mișcarea nu a persistat, stinge LED-ul
  }
}

void triggerAlarm() {
  alarmTriggered = true;
  lcd.setCursor(0, 0);
  lcd.print("Alarm: Triggered");
  while(alarmTriggered){ // Cât timp alarma este declanșată
    handleKeypad();
    if(millis() % 200 == 0){
      // Sunet de alarma si blink la fiecare 200 de milisecunde
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
      // Afișează mesajul de eroare timp de 2 secunde
      lcd.setCursor(0, 1);
      lcd.print(message);
    }
  }
  resetLcdSecondRow();
}

void beep(unsigned int duration, unsigned int frequency) {
  // Emite un sunet de o anumita frecvență pentru o anumită durată
  tone(buzzerPin, frequency);
  delay(duration);
  noTone(buzzerPin);
}

void resetLcdSecondRow(){
  // Resetează al doilea rând al LCD-ului dupa un cod greșit sau o eroare
  lcd.setCursor(0, 1);
  lcd.print("Code:         ");
}