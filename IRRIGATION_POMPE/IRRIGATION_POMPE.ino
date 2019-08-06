#include <MKRGSM.h>
#include <Servo.h>
/*
 * Ce code permet d'activer une pompe par un simple servo moteur appuyant sur un bouton, commandé par SMS.
 * Le code nécessite un MKR1400, un Servo moteur, ainsi qu'un detecteur d'activité de la pompe (micro, vibrations, ...)
 * Le servo est branché sur le PIN n°6, le detecteur de son sur la broche A1. Le seuil de bruit est situé à 190.
 * 
 */
const char PINNUMBER[] = SECRET_PINNUMBER;
const char Alex[] = ALEX;
const char Denis[] = DENIS;
const char Max[] = MAX;
const char Simon[] = SIMON ;
const char senderNumber[20];

const int seuil = 190;
bool bruit = 0;
bool etatPompe = 0;
int compteur;

GSM gsmAccess;
GSM_SMS sms;
Servo myservo;

void setup() {
  turnServo(10);
  digitalWrite(LED_BUILTIN,LOW);
  Serial.begin(9600);
  bool connected = false;
  
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("probleme initialisation");
    }
  }
  Serial.println("GSM initialized");
  char iniMsg[] = "Arduino OK : envoyer E pour demander l'etat, envoyer 1 pour allumer la pompe";
  envoiSMS(Alex, iniMsg);
  envoiSMS(Max, iniMsg);
  envoiSMS(Denis, iniMsg);
  envoiSMS(Simon, iniMsg);
  blinkLed(LED_BUILTIN,500);
}

void loop() {
  
  bruit = checkState();
  Serial.println("--- START ---");
  Serial.print("bruit : ");
  Serial.println(bruit);
  Serial.print("etatPompe : ");
  Serial.println(etatPompe);
  
  if (etatPompe == HIGH) {  // Si la pompe vient de s'éteindre
    //Serial.println("ETAT POMPE HIGH");
    while (bruit == LOW) {
      bruit = checkState();
      //Serial.println("bruit LOW ");
      compteur += 1;
      Serial.print("compteur d'eteignement : ");
      Serial.println(compteur);
      if (compteur > 10){
        etatPompe = LOW;
        compteur = 0;
        char txtMsg[] = "O Vezoune plus !";
        envoiSMS(Alex, txtMsg);
        envoiSMS(Max, txtMsg);
        envoiSMS(Denis, txtMsg);
        envoiSMS(Simon, txtMsg);
        Serial.println("la pompe s'est éteinte");
        break;
      }
    }
    compteur = 0;
  }
  if (etatPompe == LOW) {  // Si la pompe vient de s'éteindre
    //Serial.println("ETAT POMPE LOW");
    while (bruit == HIGH) {
      bruit = checkState();
      //Serial.println("bruit HIGH ");
      compteur += 1;
      Serial.print("compteur d'allumage : ");
      Serial.println(compteur);
      if (compteur > 10){
        etatPompe = HIGH;
        compteur = 0;
        char txtMsg[] = "O Vezoune ! !";
        envoiSMS(Alex, txtMsg);
        envoiSMS(Max, txtMsg);
        envoiSMS(Denis, txtMsg);
        envoiSMS(Simon, txtMsg);
        Serial.println("la pompe s'est allumée");
        break;
      }
    }
    compteur = 0;
  }
  
  if (sms.available()) {
    Serial.print("Message received from : ");
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    //Etat demandé
    if (sms.peek() == 'E') {
      Serial.println("Etat demandé");
      if (etatPompe == 1) {
        char txtMsg[] = "Etat : pompe allumee";
        envoiSMS(senderNumber, txtMsg);
        Serial.println("Message d'etat envoyé");
      }
      if (etatPompe == 0) {
        char txtMsg[] = "Etat : pompe eteinte";
        envoiSMS(senderNumber, txtMsg);
        Serial.println("Message d'etat envoyé");
      }      
    }

    //Allumange demandé
    if (sms.peek() == '1') {
      Serial.println("Allumage demandé");
      char txtMsg[] = "Allumage en cours...";
      envoiSMS(Alex, txtMsg);
      //envoiSMS(Max, txtMsg);
      //envoiSMS(Denis, txtMsg);
      //envoiSMS(Simon, txtMsg);

      int i = 0 ;
      while (etatPompe == LOW){
        turnServo(90);
        delay(4000);
        etatPompe = checkState();

        if (etatPompe == HIGH){
          char txtMsg[] = "O vezoune bien ! C'est parti !";
          envoiSMS(Alex, txtMsg);
          envoiSMS(Max, txtMsg);
          envoiSMS(Denis, txtMsg);
          envoiSMS(Simon, txtMsg);
          Serial.println("allumage effectué");

          break;
        }
        else {
          i += 1;
          if (i>1){
            Serial.println("problème d'alumage");
            char txtMsg[] = "O Vezoune pas... Verifier l'etat et reessayer";
            envoiSMS(Alex, txtMsg);
            envoiSMS(Max, txtMsg);
            envoiSMS(Denis, txtMsg);
            envoiSMS(Simon, txtMsg);
            break;
          }
        }
      }
    }
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }  
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}

// Fonction de vérification de l'état de la pompe 
bool checkState() {
  bool sound;
  int tab[100];
  long int sum = 0;
  for (int i = 0; i < 100; i++) { // lecture de 100 valeurs
    tab[i] = analogRead(A1);
    sum += tab[i];
    delay(10);
  } 
  int sumMean = 0;
  for (int i = 0; i < 100; i++) { // Centrer les données
    tab[i] = abs(tab[i] - sum/100);
    sumMean += tab[i]; 
  }
  //Serial.print(sum/100);
  //Serial.print(" ");
  //Serial.println(sumMean);
  if (sumMean > seuil) {
    sound = 1;
  }
  else (sound = 0);
  Serial.print("resultat : " );
  Serial.println(sumMean);
  Serial.print("sound : ");
  Serial.println(sound);
  return sound;
}

//Fonction Envoi de SMS
int envoiSMS (char numero[], char msg[]) {
  sms.beginSMS(numero);
  sms.print(msg);
  sms.endSMS();
}

//Fonction BlinkLed
int blinkLed (int led, int delai) {
  digitalWrite(led, HIGH);
  delay(delai);
  digitalWrite(led, LOW);
  delay(delai);
}

//Fonction active servo
void turnServo(int rotation) {
  myservo.attach(6);  //Servo sur la broche 6
  myservo.write(rotation);
  delay(1000);
  myservo.write(0);
  delay(1000);
  myservo.detach();
}
