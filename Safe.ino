#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//Servo Einstellungen ZU/AUF
#define ZU 0
#define AUF 180

//RGB PIN Zuweisung
const byte red = 11;
const byte green = 10;
const byte blue = 9;

//Buzzer Pin Zuweisung
const byte buzzer = 13;

//Servo Objekt
Servo schloss;

//LCD Objekt
LiquidCrystal_I2C lcd(0x27,20,4);

//Reihen-/Zeilenanzahl vom Zahlenfeld
const byte row = 4;
const byte col = 3;

//Tasten auf Zahlenfeld definieren
char hexaKeys[row] [col] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

//Zahlenfeld Pins entsprechend nach Datenblatt
byte rowPins[row] = {7, 2, 3, 5};
byte colPins[col] = {6, 8, 4};

//Zahlenfeld Objekt
Keypad zahlenfeld = Keypad(makeKeymap (hexaKeys), rowPins, colPins, row, col);

//Passwort standartmäßig auf 1234
String passwort = "1234";

//String für Passwortänderung
String passwortneu;

//String um Eingabe auf dem LCD anzuzeigen
String anzeige;

//Eingegebene Zahl
char zahl;

//Status geschlossen wird standardmäßig angenommen, insgesamt 3: geschlossen,offen,pwd(neues Passwort eingeben)
String status = "geschlossen";


void setup() {

  //RGB einstellen
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  //Buzzer einstellen
  pinMode(buzzer, OUTPUT);

  //Servo Pin Zuweisung
  schloss.attach(12);

  //LCD einstellen
  lcd.init();
  lcd.backlight();

  //Schloss standardmäßig zu
  schloss.write(ZU);

}


void loop() {
  
  
  //geschlossener Zustand
  if (status == "geschlossen") {
    
    eingabeZahlenfeld(); //Methodenaufruf

    //Display bearbeiten
    lcd.setCursor(0,0);
    lcd.print("Geschlossen");
    lcd.setCursor(0,1);
    lcd.print("Code : " + anzeige); //Eingabe wird hier angezeigt

    //Enter drücken
    if (zahl == '#') {
      if (anzeige == passwort) {
        korrekterCode(); //Methodenaufruf
      } else {
        falscherCode(); //Methodenaufruf
      }
    }
    //Eingabe löschen
    if (zahl == '*'){
      anzeige = "";
      lcd.clear();
    }
    delay(100); //Schleife alle 100ms

  }
  
  
  //geöffneter Status
  if (status == "offen") {

    eingabeZahlenfeld(); //Methodenaufruf

    //Display bearbeiten
    lcd.setCursor(0,0);
    lcd.print("Offen");
    lcd.setCursor(0,1);
    lcd.print("Verriegeln: #");

    //neues Passwort
    if (zahl == '*') {
      lcd.clear();
      anzeige = "";
      status = "pwd"; //Übergang zum Status pwd
    }
    //Bei Enter drücken schließen
    if (zahl == '#') {
      verriegeln();
    }
    delay(100); //Schleife alle 100ms

  }
  
  
  //Passwort ändern Status
  if (status == "pwd") {

    eingabeZahlenfeld();//Methodenaufruf

    //Display bearbeiten
    lcd.setCursor(0,0);
    lcd.print("Neues Passwort");
    lcd.setCursor(0,1);
    lcd.print(anzeige); //Neues Passwort anzeigen

    //Neues Passwort speichern
    if (zahl == '#') {
      lcd.clear();
      passwort = anzeige; //Passwort wird übernommen
      anzeige = "";
      status = "offen"; //Übergang zum Status offen
    }
    //Eingabe löschen
    if (zahl == '*') {
      lcd.clear();
      anzeige = "";
    }
    delay(100); //Schleife alle 100ms

  }

}

//Methoden Definitionen

//Zahleneingabe
void eingabeZahlenfeld() {
  zahl = zahlenfeld.getKey();
  if (zahl != '#') { // Zeichen # soll logischerweise nicht mehr dem String hinzugefügt werden
    anzeige += String(zahl); //Zahl wird dem String anzeige hinzugefügt
  }
}

//Bei Falscher Eingabe im geschlossenen Zustand
void falscherCode() {

  //Display bearbeiten
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Falscher Code");

  digitalWrite(red, HIGH); //Rotes Licht
  tone(buzzer, 120); //Tiefen Ton abspielen
  delay(1500); //Licht,Ton und Anzeige nach 1,5s ausschalten
  digitalWrite(red, LOW);
  noTone(buzzer);
  anzeige = ""; //Eingabe wird zurückgesetzt
  lcd.clear();
}

//Bei korrekter Eingabe im geschlossenen Zustand
void korrekterCode() {
  status = "offen"; //Übergang zum Status offen

  //Display bearbeiten
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Richtiger Code");

  digitalWrite(green, HIGH); //Grünes Licht
  schloss.write(AUF); //Schloss öffnen

  //Melodie abspielen
  tone(buzzer, 523,140);
  delay(150);
  tone(buzzer, 659,140);
  delay(150);
  tone(buzzer, 784,140);
  delay(150);
  tone(buzzer, 1046,140);
  delay(2000); //Licht und Anzeige nach 2s ausschalten
  digitalWrite(green, LOW);
  anzeige = "";
  lcd.clear();
}

//Schloss verriegeln bei geöffnetem Status
void verriegeln() {
  status = "geschlossen"; //Übergang zum Zustand geschlossen
  //Display bearbeiten
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Verriegelt");

  schloss.write(ZU); //Schloss verriegeln
  tone(buzzer, 400); //Ton abspielen
  digitalWrite(red, HIGH); //Rotes Licht
  delay(150); //Licht nach 150ms ausschalten
  tone(buzzer, 700,150);
  digitalWrite(red, LOW);
  delay(2000); //Anzeige nach 2s löschen
  lcd.clear();
}

