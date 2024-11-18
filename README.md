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

## Poze

## Video

Videoclipul poate fi văzut prin accesarea link-ului:
[Video pe YouTube](https://www.youtube.com/shorts/xFW5oaAeDoQ)

## Schema Circuitului
