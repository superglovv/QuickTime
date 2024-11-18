#include <SPI.h>

volatile char command = '\0';
volatile bool newCommand = false;

const int player1ButtonsPin = A1;
const int player2ButtonsPin = A0;

const int player1RedLEDPin = 5;
const int player1GreenLEDPin = 6;
const int player1BlueLEDPin = 7;

const int player2RedLEDPin = 2;
const int player2GreenLEDPin = 3;
const int player2BlueLEDPin = 4;

unsigned long pressStartTime = 0;
bool waitingForPress = true;
bool player1Turn = true;

int fastResponse = 900;
int moderateResponse = 1800;
int slowResponse = 3000;

int player2MultValues[] = {400,250,100};
int player1MultValues[] = {300,200,100};

void setup() {
  Serial.begin(115200);
  SPCR |= bit(SPE);
  pinMode(MISO, OUTPUT);
  SPI.attachInterrupt();
  
  pinMode(player1ButtonsPin, INPUT);
  pinMode(player1ButtonsPin, INPUT);

  pinMode(player1RedLEDPin, OUTPUT);
  pinMode(player1GreenLEDPin, OUTPUT);
  pinMode(player1BlueLEDPin, OUTPUT);
  pinMode(player2RedLEDPin, OUTPUT);
  pinMode(player2GreenLEDPin, OUTPUT);
  pinMode(player2BlueLEDPin, OUTPUT);
}

ISR(SPI_STC_vect) {
  char receivedChar = SPDR;
  if (receivedChar != '#') {
    command = receivedChar;
    newCommand = true;
  }
}

void loop() {
  if (newCommand) {
    newCommand = false;

    if (command == 'r' || command == 'g' || command == 'b') {
      startButtonChallenge(command);
    } else {
      SPDR = '$';
    }
  }

  if (waitingForPress) {
    checkButtonResponse();
  }
}

void startButtonChallenge(char color) {
  Serial.print("Received: ");
  Serial.print((char)SPDR);
  Serial.println(player1Turn ? " for p1" : " for p2");
  pressStartTime = millis();
  waitingForPress = true;

  activateLED(color);
}

void checkButtonResponse() {
  unsigned long elapsedTime = millis() - pressStartTime;
  
  if((player1Turn && (analogRead(player1ButtonsPin) >= player1MultValues[0] || analogRead(player1ButtonsPin) >= player1MultValues[1] || analogRead(player1ButtonsPin) >= player1MultValues[2]))
  ||
    (!player1Turn && (analogRead(player2ButtonsPin) >= player2MultValues[0] || analogRead(player2ButtonsPin) >= player2MultValues[1] || analogRead(player2ButtonsPin) >= player2MultValues[2]))) {
    
    waitingForPress = false;
  }

  if (!waitingForPress) { 
    char score;

    if (elapsedTime <= fastResponse) {
      score = 'a';
    } else if (elapsedTime <= moderateResponse) {
      score = 'b';
    } else if (elapsedTime <= slowResponse) {
      score = 'c';
    } else {
      score = 'i';
    }
    
    SPDR = score;
    
    waitingForPress = true;
  } else if (elapsedTime > slowResponse) {
    waitingForPress = true;
    SPDR = 'i';
  }
  player1Turn = !player1Turn; 
  waitingForPress = true;
}

void resetLEDs() {
  digitalWrite(player1RedLEDPin, LOW);
  digitalWrite(player1GreenLEDPin, LOW);
  digitalWrite(player1BlueLEDPin, LOW);
  digitalWrite(player2RedLEDPin, LOW);
  digitalWrite(player2GreenLEDPin, LOW);
  digitalWrite(player2BlueLEDPin, LOW);
}

void activateLED(char color) {
  resetLEDs();
  int ledPin;
  
  if (player1Turn) {
    ledPin = (color == 'r') ? player1RedLEDPin :
             (color == 'g') ? player1GreenLEDPin :
             player1BlueLEDPin;
  } else {
    ledPin = (color == 'r') ? player2RedLEDPin :
             (color == 'g') ? player2GreenLEDPin :
             player2BlueLEDPin;
  }

  digitalWrite(ledPin, HIGH);
  delay(100);
  resetLEDs();
}