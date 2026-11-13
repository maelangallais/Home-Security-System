/*
Ce code permet la sécurité maximale d'une maison.
Il fait en sorte que si la photorésistance capte qu'il fait "nuit", une Led blanche est allumée en continu, et que le PIR Motion Sensor Module détecte du mouvement (alors cela enclenchera une alarme sonore avec le Passive Buzzer),
ainsi que le clignotement de la Led blanche, et pour déverouiller l'alarme et ouvrir le "portail", il y aura 4 étapes qui déclencheront toutes une action qui leur sera propre.
Durant l'initialisation, une Led rouge est allumée pour signifier que l'initialisation n'est pas terminée, le Servomoteur indique une barre de chargement, allant de 0 à 180. Une fois terminé la Led verte s'allume.
Il faudra d'abord scanner un badge pour désactiver l'alarme.
Puis en tapant un code sur la Membrane Switch Module, une Led RGB va passer de rouge à vert, en passant par le orange(, cela déclenchera une sonnerie d'approbation avec le Pasive Buzzer).
Ensuite il faudra effectuer une suite de touches précises sur la télécommande captées par l'IR Receiver Module, qui va afficher, en alternance, un logo vérifié et un smiley avec le Max7219 Module.
Enfin, il faudra appuyer sur une suite de 5 buttons dans un ordre précis, qui ouvrira cette fois le "portail" représenté par le Servo Moteur.
L'écran LCD affichera toute les instructions dans cet ordre précis et cela allumera des Led rouges, jaune, verte puis bleu au fur et à mesure du passage des sécurités.
A chaque demande de confirmation d'un module, une Led rouge ou verte s'allumera en fonction de si l'utilisateur a saisi le bon mot de passe ou non.
La bibliothèque de l'Active Buzzer n'est pas compatible avec celle de l'IRemote. De plus, il est impossible d'effectuer deux actions en parallèles, même avec une bibliothèque comme TaskScheduler.
 */

#include "LedControl.h"
#include "IRremote.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

const byte Photocell=A0;
const byte LedBlanche=25;
const byte PIR=48;
const byte pinRST=49;
const byte pinSDA=20;
const byte ActiveBuzzer=44;
const byte IRemote=47;
const byte LedRedGB=3;
const byte LedRGreenB=4;
const byte LedRGBlue=5;
LedControl lc=LedControl(22,23,24,1);
const byte Button1=36;
const byte Button2=37;
const byte Button3=38;
const byte Button4=39;
const byte Button5=40;
const byte LedRouge=26;
const byte LedJaune=28;
const byte LedVert=27;
const byte LedBleu=29;
LiquidCrystal lcd(30,31,32,33,34,35);
const byte lignes=4;
const byte colonnes=4;
byte PinsLignes[lignes]={46,10,8,7};
byte PinsColonnes[colonnes]={41,42,43,45};
const byte LedConfirmation=53;
const byte LedRefus=12;
const byte RgbLed=13;
const byte rGbLed=6;
const byte rgBLed=9;

IRrecv irrecv(IRemote);
decode_results results;
MFRC522 RFID(pinSDA,pinRST);
Servo Microservo;
char Boutons[lignes][colonnes]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
Keypad Membrane=Keypad(makeKeymap(Boutons),PinsLignes,PinsColonnes,lignes,colonnes);
byte RedGBValue;
byte RGreenBValue;
byte RGBlueValue;

void setup(){
  Serial.begin(9600);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  pinMode(ActiveBuzzer,OUTPUT);
  pinMode(PIR,INPUT);
  pinMode(Button1,INPUT_PULLUP);
  pinMode(Button2,INPUT_PULLUP);
  pinMode(Button3,INPUT_PULLUP);
  pinMode(Button4,INPUT_PULLUP);
  pinMode(Button5,INPUT_PULLUP);
  pinMode(LedRouge,OUTPUT);
  pinMode(LedJaune,OUTPUT);
  pinMode(LedVert,OUTPUT);
  pinMode(LedBleu,OUTPUT);
  pinMode(LedBlanche,OUTPUT);
  SPI.begin();
  RFID.PCD_Init();
  irrecv.enableIRIn();
  lcd.begin(16, 2);
  pinMode(LedRedGB,OUTPUT);
  pinMode(LedRGreenB,OUTPUT);
  pinMode(LedRGBlue,OUTPUT);
  pinMode(RgbLed,OUTPUT);
  pinMode(rGbLed,OUTPUT);
  pinMode(rgBLed,OUTPUT);
  pinMode(LedConfirmation,OUTPUT);
  pinMode(LedRefus,OUTPUT);
  Microservo.attach(11);
  digitalWrite(LedConfirmation,LOW);
  digitalWrite(LedRefus,HIGH);
  lcd.print("Systeme securite");
  lcd.setCursor(0,1);
  lcd.print("initialisation..");
  for(int i=0;i<180;i++){
    Microservo.write(i);
    delay(15);
  }
  digitalWrite(LedRefus,LOW);
  digitalWrite(LedConfirmation,HIGH);
  delay(1000);
  ClearAll();
}

void Max(){
  byte logo[8]={B00111100,B01000010,B10010001,B10100001,B10010001,B10001001,B01000010,B00111100};
  byte smiley[8]={B00111100,B01000010,B10010101,B10100001,B10100001,B10010101,B01000010,B00111100};
  for(byte i=0;i<8;i++){
    lc.setRow(0,i,smiley[i]);
  }
  delay(500);
  for(int i=0;i<8;i++){
    lc.setRow(0,i,logo[i]);
  }
  delay(500);
}

void ClearAll(){
  lcd.clear();
  lc.clearDisplay(0);
  digitalWrite(LedRouge,LOW);
  digitalWrite(LedBlanche,LOW);
  digitalWrite(LedRedGB,LOW);
  digitalWrite(LedRGreenB,LOW);
  digitalWrite(LedBlanche,LOW);
  digitalWrite(LedRGBlue,LOW);
  digitalWrite(RgbLed,LOW);
  digitalWrite(rGbLed,LOW);
  digitalWrite(rgBLed,LOW);
  digitalWrite(LedConfirmation,LOW);
  digitalWrite(LedRefus,LOW);
  digitalWrite(LedBleu,LOW);
  Microservo.write(90);
  digitalWrite(LedVert,LOW);
  digitalWrite(LedJaune,LOW);
  lcd.print("Bonjour !");
  lcd.setCursor(0,1);
  lcd.print("Bienvenue !");
}

long unsigned int HEXBoutonsTelecommande[22]={0xFFFFFFFF,
  0xFFA25D,0xFF629D,0xFFE21D,
  0xFF22DD,0xFF02FD,0xFFC23D,
  0xFFE01F,0xFFA857,0xFF906F,
  0xFF6897,0xFF9867,0xFFB04F,
  0xFF30CF,0xFF18E7,0xFF7A85,
  0xFF10EF,0xFF38C7,0xFF5AA5,
  0xFF42BD,0xFF4AB5,0xFF52AD,
};

const char* telecommande[22]={"Erreur systeme!",
  "Allumer/Eteindre","Volume +","Fonction/Stop",
  "Arriere rapide","Pause","Avance rapide",
  "Bas","Volume -","Haut",
  "0","EQ","ST/REPT",
  "1","2","3",
  "4","5","6",
  "7","8","9",
};

const byte UIDbadge[4]={19,174,7,183};
const byte UIDcarte[4]={241,223,169,27};
const char* chiffre[5]={"1er","2eme","3eme","4eme","5eme"};
char codeSecretMembrane[4]={'1','2','3','4'};
char codeSaisiMembrane[4];
const char* codeSecretRemote[4]={"1","2","3","4"};
char* codeSaisiRemote[4];
byte codeSecretButton[5]={1,2,3,4,5};
byte codeSaisiButton[5];

void loop(){
  ClearAll();
  while(true){
    bool Stage2=false;
    bool Stage3=false;
    bool Stage4=false;
    int luminosity=analogRead(Photocell);
    lcd.setCursor(0,1);
    lcd.print("Luminosite : ");
    lcd.print(luminosity);
    if(luminosity<=80){
      digitalWrite(LedBlanche,HIGH);
      bool EtatPIR=false;
      bool EtatActuelPIR = digitalRead(PIR);
      if (EtatActuelPIR!=EtatPIR && EtatActuelPIR == HIGH){
        lcd.clear();
        lcd.print("Presentez le");
        lcd.setCursor(0,1);
        lcd.print("support !");
        while (RFID.PICC_IsNewCardPresent()!=HIGH){
          digitalWrite(LedBlanche,HIGH);
          digitalWrite(ActiveBuzzer,HIGH);
          delay(100);
          digitalWrite(LedBlanche,LOW);
          digitalWrite(ActiveBuzzer,LOW);
          delay(100);
        }
        bool BadgeLu=true;
        while(BadgeLu){
          if(RFID.PICC_IsNewCardPresent()){
            if(RFID.PICC_ReadCardSerial()){
              BadgeLu=false;
              int RefusBadge=0;
              int RefusCarte=0;
              for(byte i=0;i<RFID.uid.size;i++){
                Serial.println(RFID.uid.uidByte[i]);
                if(RFID.uid.uidByte[i]!=UIDbadge[i]){
                  RefusBadge++;
                }
              }
              for(byte i=0;i<RFID.uid.size;i++){
                Serial.println(RFID.uid.uidByte[i]);
                if(RFID.uid.uidByte[i]!=UIDcarte[i]){
                  RefusCarte++;
                }
              }
              if(RefusBadge==0||RefusCarte==0){
                lcd.clear();
                if(RefusBadge==0){
                  lcd.print("Badge passe");
                  lcd.setCursor(0,1);
                  lcd.print("avec succes !");
                }
                else if(RefusCarte==0){
                  lcd.print("Carte passee");
                  lcd.setCursor(0,1);
                  lcd.print("avec succes !");
                }
                digitalWrite(LedConfirmation,HIGH);
                digitalWrite(LedRouge,HIGH);
                Stage2=true;
                delay(3000);
                lcd.clear();
                digitalWrite(LedConfirmation,LOW);
              }else{
                digitalWrite(LedRefus,HIGH);
                lcd.print("Vous n'avez pas");
                lcd.setCursor(0,1);
                lcd.print("l'autorisation !");
                delay(3000);
                break;
              }
            }else{
              lcd.clear();
              lcd.print("Support");
              lcd.setCursor(0,1);
              lcd.print("illisible !");
              delay(2000);
            }
          }else{
            lcd.clear();
            lcd.print("Essayez de repas");
            lcd.setCursor(0,1);
            lcd.print("ser le support !");
          }
        }
        if(Stage2){
          bool codeValide=true;
          lcd.print("Tapez code sur");
          lcd.setCursor(0,1);
          lcd.print("pave numerique.");
          delay(2000);
          lcd.clear();
          for (int i=0;i<4;i++){
            lcd.print("Entrez le ");
            lcd.print(chiffre[i]);
            lcd.setCursor(0,1);
            lcd.print("chiffre : ");
            char key=Membrane.getKey();
            while(key==NO_KEY){
              key=Membrane.getKey();
            }
            codeSaisiMembrane[i]=key;
            lcd.print(codeSaisiMembrane[i]);
            delay(500);
            lcd.clear();
          }
          for(int i=0;i<4;i++){
            if(codeSaisiMembrane[i]!=codeSecretMembrane[i]){
              codeValide=false;
              break;
            }
          }
          if(codeValide){
            lcd.print("Code bon !");
            digitalWrite(LedConfirmation,HIGH);
            digitalWrite(LedJaune,HIGH);
            Stage3=true;
            delay(3000);
            lcd.clear();
            digitalWrite(LedConfirmation,LOW);
          }else{
            lcd.print("Code errone !");
            digitalWrite(LedRefus,HIGH);
            delay(3000);
            break;
          }
        }
        if(Stage3){
          bool codeValide=true;
          lcd.print("Interaction avec");
          lcd.setCursor(0,1);
          lcd.print("la telecommande.");
          delay(2000);
          analogWrite(rGbLed,0);
          for(int i=0;i<4;i++){
            lcd.clear();
            lcd.print("Entrez le ");
            lcd.print(chiffre[i]);
            lcd.print(" :");
            bool signalRecu = false;
            while(!signalRecu){
              if(irrecv.decode(&results)){
                for(int a=0;a<22;a++){
                  if(results.value==HEXBoutonsTelecommande[a]){
                    lcd.setCursor(0,1);
                    lcd.print(telecommande[a]);
                    codeSaisiRemote[i]=const_cast<char*>(telecommande[a]);
                    signalRecu = true;
                    break;
                  }
                }
                delay(500);
                irrecv.resume();
              }
            }
          }
          for(int i=0;i<4;i++){
            if(codeSaisiRemote[i]!=codeSecretRemote[i]){
              codeValide=false;
              break;
            }
          }
          if(codeValide){
            lcd.clear();
            lcd.print("Code bon !");
            digitalWrite(LedConfirmation,HIGH);
            digitalWrite(LedVert,HIGH);
            Stage4=true;
            for(int i=0;i<3;i++){
              Max();
            }
            delay(3000);
            lcd.clear();
            digitalWrite(LedConfirmation,LOW);
          }else{
            lcd.clear();
            lcd.print("Code errone !");
            digitalWrite(LedRefus,HIGH);
            delay(3000);
            break;
          }
        }
        if(Stage4){
          bool codeValide=true;
          int a=0;
          lcd.print("Interaction avec");
          lcd.setCursor(0,1);
          lcd.print("les 5 boutons.");
          delay(2000);
          for(int i=0;i<5;i++){
            lcd.clear();
            lcd.print("Appuyez sur le");
            lcd.setCursor(0,1);
            lcd.print(chiffre[i]);
            lcd.print(" bouton !");
            bool buttonPressed=true;
            while(buttonPressed){
              if(digitalRead(Button1)==LOW){
                codeSaisiButton[i]=1;
                a=1;
                buttonPressed=false;
              }
              else if(digitalRead(Button2)==LOW){
                codeSaisiButton[i]=2;
                a=2;
                buttonPressed=false;
              }
              else if(digitalRead(Button3)==LOW){
                codeSaisiButton[i]=3;
                a=3;
                buttonPressed=false;
              }
              else if(digitalRead(Button4)==LOW){
                codeSaisiButton[i]=4;
                a=4;
                buttonPressed=false;
              }
              else if(digitalRead(Button5)==LOW){
                codeSaisiButton[i]=5;
                a=5;
                buttonPressed=false;
              }
            }
            lcd.clear();
            lcd.print("Bouton numero");
            lcd.setCursor(0,1);
            lcd.print(a);
            lcd.print(" presse !");
            delay(1000);
          }
          for(int i=0;i<5;i++){
            if(codeSaisiButton[i]!=codeSecretButton[i]){
              codeValide=false;
              break;
            }
          }
          if(codeValide){
            lcd.clear();
            lcd.print("Suite correcte !");
            lcd.setCursor(0,1);
            lcd.print("Open the portal!");
            digitalWrite(LedConfirmation,HIGH);
            digitalWrite(LedBleu,HIGH);
            for(int i=90;i>0;i--){
              Microservo.write(i);
              delay(50);
            }
            delay(3000);
            for(byte i=0;i<255;i++){
              analogWrite(LedRedGB,i);
              delay(20);
            }
            for(byte i=0;i<255;i++){
              analogWrite(LedRedGB,255-i);
              analogWrite(LedRGreenB,i);
              delay(20);
            }
            for(byte i=0;i<255;i++){
              analogWrite(LedRGreenB,255-i);
              analogWrite(LedRGBlue,i);
              delay(20);
            }
            delay(1000);
            break;
          }else{
            lcd.clear();
            lcd.print("Suite");
            lcd.setCursor(0,1);
            lcd.print("incorrecte !");
            digitalWrite(LedRefus,HIGH);
            delay(3000);
            break;
          }
        }
      }
    }else{
      digitalWrite(LedBlanche,HIGH);
      delay(500);
      digitalWrite(LedBlanche,LOW);
      delay(500);
    }
  }
  delay(500);
}
