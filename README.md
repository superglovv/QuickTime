# Joc de tip QuickTime

## Descrierea Task-ului

Această temă presupune dezvoltarea unui joc interactiv de reflexe, destinat a doi jucători, care își testează viteza de reacție prin apăsarea rapidă a butonului asociat culorii afișate pe LED-ul RGB alocat fiecărui jucător.

Pe parcursul unei runde, jucătorii primesc pe rând o culoare aleatorie (roșu, verde sau albastru). Fiecare trebuie să apese rapid butonul corect pentru a obține puncte. Scorurile sunt actualizate automat și afișate pe un ecran LCD. Jocul se încheie după ce servomotorul efectuează o rotație completă, iar scorurile finale sunt afișate.

## Componente Utilizate

- 2x Arduino Uno
- 3x Breadboard
- 2x LED RGB
- 6x LED-uri (2x LED albastru, 2x LED verde, 2x LED roșu)
- Buton Start
- 6x Butoane corespunzatoare culorii de apasat (3 pentru fiecare jucator) (Pentru economisirea pinilor, am realizat multiplexarea butoanelor, acestea fiind conectate printr-un sistem de rezistențe - 4 butoane legate în serie și conectate la A1 pe primul breadboard și 3 butoane pe cel de-al doilea breadboard, conectate in serie și la A0)
- Ecran LCD
- Servomotor
- 22x Rezistențe (13x 220 Ohm, 9x 330 Ohm)
- Cabluri de conectare

## Principiul de Funcționare

### Inițializarea jocului

Jucătorii pot porni jocul prin apăsarea butonului de start, care este primul buton multiplexat de pe primul breadboard. După detectarea unui semnal valid pe pinul butonului, Master-ul solicită fiecărui jucător să își introducă numele folosind monitorul serial. Numele introduse sunt afișate pe ecranul LCD atașat plăcii Master, iar jocul trece în modul activ.

Master-ul inițializează cronometrul jocului utilizând funcția millis() și setează durata totală a unei runde (gameDuration - 30s). În același timp, servomotorul conectat la placa Master începe să indice progresul jocului, rotindu-se gradual de la 0 la 180 de grade în funcție de timpul scurs.

### Desfășurarea jocului

Principiul de funcționare
Acest sistem implementează un joc de reflexe pentru doi jucători, utilizând două plăci Arduino (Master și Slave) care comunică între ele prin protocolul SPI. Jocul combină elemente hardware precum butoane, LED-uri și un servomotor, cu o logică software bine definită pentru gestionarea rundelor și punctajului.

Inițializarea Jocului
Jocul începe prin apăsarea unui buton conectat la placa Master. După detectarea unui semnal valid pe pinul butonului, Master-ul solicită fiecărui jucător să își introducă numele folosind monitorul serial. Numele introduse sunt afișate pe ecranul LCD atașat plăcii Master, iar jocul trece în modul activ.

Master-ul inițializează cronometrul jocului utilizând funcția millis() și setează durata totală a unei runde. În același timp, servomotorul conectat la placa Master începe să indice progresul jocului, rotindu-se gradual de la 0 la 180 de grade în funcție de timpul scurs.

Desfășurarea Rundelor
Fiecare rundă este alternată între cei doi jucători. La începutul unei runde, Master-ul alege o culoare aleatorie (roșu, verde sau albastru) și o transmite către Slave prin SPI. Slave-ul aprinde LED-ul corespunzător culorii selectate, fie pentru jucătorul 1, fie pentru jucătorul 2, în funcție de a cui este rândul.

Jucătorul activ trebuie să apese un buton care să corespundă culorii afișate. Slave-ul monitorizează răspunsul butonului și măsoară timpul de reacție al jucătorului. Dacă butonul corect este apăsat, Slave-ul calculează punctajul pe baza timpului de răspuns:

Timp rapid: 5 puncte.
Timp moderat: 3 puncte.
Timp lent: 1 punct.
Răspuns incorect sau întârziat: 0 puncte.
Punctajul calculat este transmis înapoi la Master prin SPI. Master-ul actualizează scorul jucătorului și afișează scorurile curente pe ecranul LCD.

### Gestionarea Cronometrului

Servomotorul conectat la placa Master acționează ca un indicator vizual al progresului jocului. Mișcarea acestuia este direct proporțională cu timpul scurs din durata totală a jocului. În momentul în care servomotorul ajunge la capătul rotației (180 de grade), durata jocului s-a încheiat, iar Master-ul oprește rundele și afișează rezultatele finale.

### Finalizarea Jocului

La sfârșitul jocului, Master-ul afișează scorurile finale ale ambilor jucători pe LCD și declară câștigătorul în funcție de punctaj. În caz de egalitate, se afișează un mesaj corespunzător. După afișarea rezultatelor, jocul se resetează automat, fiind pregătit pentru o nouă sesiune.

### Comunicația SPI

- Arduino Master:
  Controlează LCD-ul, servomotorul și logica principală a jocului.
  Trimite către Slave mesajele legate de culoarea LED-ului RGB și înregistrează punctajele.

- Slave Arduino:
  Controlează LED-urile și verifică butoanele.
  Trimite către Master informații despre apăsările de butoane.

## Explicatia codului

### Master:

```cpp
#include <SPI.h>                // Biblioteca SPI pentru comunicarea cu dispozitive slave
#include <LiquidCrystal.h>      // Biblioteca LiquidCrystal pentru controlul afișajului LCD
#include <Servo.h>              // Biblioteca Servo pentru controlul unui servo motor

LiquidCrystal lcd(8, 7, 6, 5, 4, 3); // Creează un obiect LCD cu pini specificați
Servo gameTimerServo;               // Creează un obiect Servo pentru timerul jocului

// Constante pentru pini și configurație
const int startPin = A1;            // Pinul pentru butonul de start
const int startValue = 700;         // Valoare de prag pentru detectarea apăsării butonului
const int servoPin = 9;             // Pinul pentru servo motor
const int servoMaxAngle = 180;      // Unghiul maxim pentru servo motor
const unsigned long gameDuration = 30000; // Durata jocului în milisecunde (30 secunde)

// Variabile ale jocului
char colors[] = {'r', 'g', 'b'};    // Culorile posibile trimise către slave
unsigned long gameStartTime = 0;    // Momentul în care jocul a început
unsigned long lastRoundTime = 0;    // Momentul în care a început ultima rundă
unsigned long roundInterval = 3000; // Intervalul de timp între runde (3 secunde)
int roundNumber = gameDuration / roundInterval; // Numărul total de runde
bool gameStarted = false;           // Indicator care arată dacă jocul a început
bool awaitingResponse = false;      // Indicator care arată dacă se așteaptă un răspuns de la slave
char response;                      // Răspunsul primit de la slave

// Detalii despre jucători
String player1, player2;            // Numele celor doi jucători
int player1Score = 0;               // Scorul jucătorului 1
int player2Score = 0;               // Scorul jucătorului 2
int roundCounter = 0;               // Numărul rundei curente
bool isPlayer1Turn = true;          // Indicator care arată a cui este rândul

void setup() {
  Serial.begin(115200);             // Inițializează comunicația serială la 115200 baud
  SPI.begin();                      // Inițializează comunicația SPI
  pinMode(SS, OUTPUT);              // Configurează pinul Slave Select ca ieșire
  digitalWrite(SS, HIGH);           // Menține Slave Select ridicat (inactiv)
  pinMode(startPin, INPUT);         // Configurează pinul butonului de start ca intrare

  lcd.begin(16, 2);                 // Inițializează afișajul LCD cu dimensiunea 16x2
  gameTimerServo.attach(servoPin);  // Atașează servo motorul la pinul specificat

  lcd.print("Press to Start!");     // Afișează mesajul inițial pe LCD
}

void loop() {
  // Pornește jocul dacă butonul este apăsat și jocul nu a început
  if (!gameStarted && analogRead(startPin) > startValue) {
    delay(200);                     // Întârziere pentru debounce
    gameStarted = true;             // Setează indicatorul că jocul a început
    lcd.clear();                    // Curăță afișajul LCD

    lcd.print("Enter Player 1:");   // Cere numele Jucătorului 1
    while (Serial.available() == 0) {} // Așteaptă introducerea numelui
    player1 = Serial.readStringUntil('\n'); // Citește numele Jucătorului 1 din serial

    lcd.clear();                    // Curăță afișajul LCD
    lcd.print("Enter Player 2:");   // Cere numele Jucătorului 2
    while (Serial.available() == 0) {} // Așteaptă introducerea numelui
    player2 = Serial.readStringUntil('\n'); // Citește numele Jucătorului 2 din serial

    gameStartTime = millis();       // Stochează momentul în care jocul a început
  }

  if (gameStarted) {                // Dacă jocul a început
    unsigned long currentMillis = millis(); // Obține timpul curent

    // Oprește jocul dacă durata a expirat
    if (currentMillis - gameStartTime >= gameDuration) {
      gameStarted = false;          // Resetează indicatorul că jocul a început
      displayWinner();              // Afișează câștigătorul
      resetGame();                  // Resetează variabilele jocului
      return;                       // Iese din iterația curentă a buclei
    }

    // Actualizează poziția servo-ului în funcție de timpul trecut
    int angle = map(currentMillis - gameStartTime, 0, gameDuration, 0, servoMaxAngle);
    gameTimerServo.write(angle);    // Mută servo-ul la unghiul calculat

    // Pornește o nouă rundă dacă a trecut suficient timp de la ultima rundă
    if (currentMillis - lastRoundTime >= roundInterval) {
      if (roundCounter < roundNumber) { // Verifică dacă mai sunt runde disponibile
        lcd.clear();                // Curăță afișajul LCD
        lcd.print((isPlayer1Turn ? player1 : player2) + " s:" + 
                  (isPlayer1Turn ? player1Score : player2Score) + " R"); // Afișează jucătorul curent și scorul
        sendColorCommand();         // Trimite o comandă cu o culoare către slave
        delay(3000);                // Așteaptă răspunsul jucătorului
        awaitingResponse = true;    // Setează indicatorul că se așteaptă un răspuns
        lastRoundTime = currentMillis; // Actualizează timpul ultimei runde
        isPlayer1Turn = !isPlayer1Turn; // Schimbă rândul între jucători
        roundCounter++;             // Crește numărul rundei
      }
    }

    if (awaitingResponse) {         // Dacă se așteaptă un răspuns de la slave
      response = sendCommand('#');  // Solicită răspunsul de la slave
      Serial.println((isPlayer1Turn ? player2 : player1) + " Received score: ");
      Serial.println(response);     // Afișează scorul primit pe serial

      updateScore(response);        // Actualizează scorul în funcție de răspuns
      awaitingResponse = false;     // Resetează indicatorul de răspuns
    }
  }
}

void sendColorCommand() {
  char colorCommand = colors[random(0, 3)]; // Alege o culoare aleatorie
  sendCommand(colorCommand);        // Trimite comanda de culoare către slave
  Serial.print("Sent color: ");     // Afișează culoarea trimisă
  Serial.println(colorCommand);
}

void updateScore(char rating) {
  int points = 0;                   // Inițializează punctajul
  switch (rating) {                 // Atribuie puncte în funcție de răspuns
    case 'a': points = 5; break;    // Răspuns rapid
    case 'b': points = 3; break;    // Răspuns mediu
    case 'c': points = 1; break;    // Răspuns lent
    case 'i': points = 0; break;    // Fără răspuns
  }

  if (!isPlayer1Turn) {
    player1Score += points;         // Adaugă puncte la Jucătorul 1
  } else {
    player2Score += points;         // Adaugă puncte la Jucătorul 2
  }
}
// Funcție pentru trimiterea unei comenzi către dispozitivul slave
char sendCommand(char command) {
  digitalWrite(SS, LOW);            // Activează comunicarea cu slave (SS jos)
  char response = SPI.transfer(command); // Trimite comanda și primește răspunsul
  digitalWrite(SS, HIGH);           // Dezactivează comunicarea cu slave (SS sus)
  return response;                  // Returnează răspunsul primit de la slave
}

// Funcție pentru afișarea câștigătorului pe LCD
void displayWinner() {
  lcd.clear();                      // Curăță afișajul LCD
  lcd.print(player1 + " score:");   // Afișează scorul Jucătorului 1
  lcd.setCursor(0, 1);              // Mută cursorul pe linia a doua
  lcd.print(player1Score);          // Afișează scorul numeric al Jucătorului 1
  delay(2000);                      // Pauză pentru a permite vizualizarea

  lcd.clear();                      // Curăță afișajul LCD
  lcd.print(player2 + " score:");   // Afișează scorul Jucătorului 2
  lcd.setCursor(0, 1);              // Mută cursorul pe linia a doua
  lcd.print(player2Score);          // Afișează scorul numeric al Jucătorului 2
  delay(2000);                      // Pauză pentru a permite vizualizarea

  lcd.clear();                      // Curăță afișajul LCD
  if (player1Score > player2Score) { // Verifică cine a câștigat
    lcd.print("Winner: " + player1); // Afișează Jucătorul 1 ca învingător
  } else if (player2Score > player1Score) {
    lcd.print("Winner: " + player2); // Afișează Jucătorul 2 ca învingător
  } else {
    lcd.print("It's a Draw!");      // Afișează mesaj de egalitate
  }
  delay(2000);                      // Pauză pentru a permite vizualizarea
}

// Funcție pentru resetarea jocului
void resetGame() {
  player1Score = 0;                 // Resetează scorul Jucătorului 1
  player2Score = 0;                 // Resetează scorul Jucătorului 2
  roundCounter = 0;                 // Resetează numărul de runde
  isPlayer1Turn = true;             // Resetează rândul la Jucătorul 1
  lcd.clear();                      // Curăță afișajul LCD
  lcd.print("Press to Start!");     // Afișează mesajul inițial pe LCD
}

```

### Slave:

```cpp
#include <SPI.h>

// Variabile volatile utilizate în ISR pentru stocarea comenzii și identificarea unei noi comenzi
volatile char command = '\0';      // Variabilă pentru a stoca comanda primită prin SPI
volatile bool newCommand = false; // Indicator pentru a marca o comandă nouă

// Pines pentru butoanele jucătorilor
const int player1ButtonsPin = A1; // Pinul pentru butoanele Jucătorului 1
const int player2ButtonsPin = A0; // Pinul pentru butoanele Jucătorului 2

// Pines pentru LED-urile Jucătorului 1
const int player1RedLEDPin = 5;
const int player1GreenLEDPin = 6;
const int player1BlueLEDPin = 7;

// Pines pentru LED-urile Jucătorului 2
const int player2RedLEDPin = 2;
const int player2GreenLEDPin = 3;
const int player2BlueLEDPin = 4;

// Variabile pentru cronometrare și starea jocului
unsigned long pressStartTime = 0;   // Momentul începerii unei provocări
bool waitingForPress = true;        // Indicator pentru așteptarea unui răspuns la provocare
bool player1Turn = true;            // Indicator pentru a urmări rândul jucătorilor

// Praguri de timp pentru evaluarea răspunsurilor
int fastResponse = 900;             // Răspuns rapid (<= 900 ms)
int moderateResponse = 1800;        // Răspuns moderat (<= 1800 ms)
int slowResponse = 3000;            // Răspuns lent (<= 3000 ms)

// Valorile pragurilor de analogRead pentru fiecare jucător
int player2MultValues[] = {400, 250, 100}; // Praguri pentru Jucătorul 2
int player1MultValues[] = {300, 200, 100}; // Praguri pentru Jucătorul 1

void setup() {
  Serial.begin(115200);             // Inițializează comunicația serială
  SPCR |= bit(SPE);                 // Activează modul SPI în microcontroler
  pinMode(MISO, OUTPUT);            // Configurează MISO ca ieșire
  SPI.attachInterrupt();            // Activează întreruperea SPI
  
  // Configurare pines pentru butoane
  pinMode(player1ButtonsPin, INPUT);
  pinMode(player2ButtonsPin, INPUT);

  // Configurare pines pentru LED-uri
  pinMode(player1RedLEDPin, OUTPUT);
  pinMode(player1GreenLEDPin, OUTPUT);
  pinMode(player1BlueLEDPin, OUTPUT);
  pinMode(player2RedLEDPin, OUTPUT);
  pinMode(player2GreenLEDPin, OUTPUT);
  pinMode(player2BlueLEDPin, OUTPUT);
}

// Rutina de întrerupere pentru recepționarea comenzilor SPI
ISR(SPI_STC_vect) {
  char receivedChar = SPDR;          // Citește comanda primită prin SPI
  if (receivedChar != '#') {         // Ignoră caracterul '#' dacă apare
    command = receivedChar;          // Stochează comanda primită
    newCommand = true;               // Marchează faptul că există o comandă nouă
  }
}

void loop() {
  if (newCommand) {                  // Dacă există o comandă nouă
    newCommand = false;              // Resetează indicatorul de comandă nouă

    if (command == 'r' || command == 'g' || command == 'b') {
      startButtonChallenge(command); // Pornește provocarea pentru apăsarea butonului
    } else {
      SPDR = '$';                    // Trimite caracterul '$' pentru comandă invalidă
    }
  }

  if (waitingForPress) {             // Verifică dacă se așteaptă un răspuns
    checkButtonResponse();
  }
}

// Funcție pentru pornirea unei provocări de apăsare a butonului
void startButtonChallenge(char color) {
  Serial.print("Received: ");        // Afișează comanda primită pentru testare
  Serial.print((char)SPDR);
  Serial.println(player1Turn ? " for p1" : " for p2");
  pressStartTime = millis();         // Stochează timpul de început al provocării
  waitingForPress = true;            // Activează așteptarea unui răspuns

  activateLED(color);                // Activează LED-ul corespunzător culorii
}

// Funcție pentru verificarea răspunsului jucătorilor
void checkButtonResponse() {
  unsigned long elapsedTime = millis() - pressStartTime; // Timpul scurs de la începerea provocării
  
  // Verifică dacă un jucător a apăsat butonul în timpul permis
  if((player1Turn && (analogRead(player1ButtonsPin) >= player1MultValues[0] || 
                      analogRead(player1ButtonsPin) >= player1MultValues[1] || 
                      analogRead(player1ButtonsPin) >= player1MultValues[2]))
  ||
    (!player1Turn && (analogRead(player2ButtonsPin) >= player2MultValues[0] || 
                      analogRead(player2ButtonsPin) >= player2MultValues[1] || 
                      analogRead(player2ButtonsPin) >= player2MultValues[2]))) {
    
    waitingForPress = false;         // Se oprește așteptarea pentru răspuns
  }

  if (!waitingForPress) {            // Dacă răspunsul a fost primit
    char score;

    if (elapsedTime <= fastResponse) {
      score = 'a';                   // Răspuns rapid -> Scor maxim
    } else if (elapsedTime <= moderateResponse) {
      score = 'b';                   // Răspuns moderat -> Scor mediu
    } else if (elapsedTime <= slowResponse) {
      score = 'c';                   // Răspuns lent -> Scor minim
    } else {
      score = 'i';                   // Răspuns invalid
    }
    
    SPDR = score;                    // Trimite scorul prin SPI
    waitingForPress = true;          // Resetează așteptarea pentru următoarea rundă
  } else if (elapsedTime > slowResponse) { 
    waitingForPress = true;          // Dacă timpul de răspuns a expirat, marchează răspuns invalid
    SPDR = 'i';                      // Trimite răspunsul invalid
  }
  player1Turn = !player1Turn;        // Schimbă rândul între jucători
  waitingForPress = true;            // Se pregătește pentru următoarea provocare
}

// Funcție pentru resetarea LED-urilor
void resetLEDs() {
  digitalWrite(player1RedLEDPin, LOW);
  digitalWrite(player1GreenLEDPin, LOW);
  digitalWrite(player1BlueLEDPin, LOW);
  digitalWrite(player2RedLEDPin, LOW);
  digitalWrite(player2GreenLEDPin, LOW);
  digitalWrite(player2BlueLEDPin, LOW);
}

// Funcție pentru activarea LED-ului corespunzător culorii date
void activateLED(char color) {
  resetLEDs();                       // Resetează toate LED-urile
  int ledPin;
  
  // Alege LED-ul corespunzător jucătorului și culorii primite
  if (player1Turn) {
    ledPin = (color == 'r') ? player1RedLEDPin :
             (color == 'g') ? player1GreenLEDPin :
             player1BlueLEDPin;
  } else {
    ledPin = (color == 'r') ? player2RedLEDPin :
             (color == 'g') ? player2GreenLEDPin :
             player2BlueLEDPin;
  }

  digitalWrite(ledPin, HIGH);        // Activează LED-ul selectat
  delay(100);                        // Pauză scurtă pentru vizualizare
  resetLEDs();                       // Resetează toate LED-urile
}

```

## Poze

![4](https://github.com/user-attachments/assets/8c4df5d3-3da6-4caf-a5e5-4692c48dfa31)
![3](https://github.com/user-attachments/assets/b7da0a99-2d88-468b-ab46-cf42b5d251e4)
![2](https://github.com/user-attachments/assets/39198762-55cb-4aaa-b21b-2847f12c5dbf)
![1](https://github.com/user-attachments/assets/14eac01e-5dac-4efe-a6da-10e1220d1748)


## Video

Videoclipul poate fi văzut prin accesarea link-ului:
[Video pe YouTube](https://www.youtube.com/shorts/xFW5oaAeDoQ)

## Schema Circuitului

![WhatsApp Image 2024-11-18 at 15 39 30_5aace553](https://github.com/user-attachments/assets/9f94491c-1c9b-4338-a95c-b5df2c56458e)
