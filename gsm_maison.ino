
#include <SoftwareSerial.h>
#include <Servo.h>

//Creation des objets
Servo myservo;
SoftwareSerial SIM900(7, 8); 

//Declaration des constantes
const int seuil = 160;
bool bruit = 0;
bool etatPompe = 0;
int compteur;
char incoming_char=0;
String msg;

void setup() {
  myservo.attach(9);  //Servo sur la broche 9
  myservo.write(0);  // Servo en position basse
  SIM900.begin(19200);  //Initiation SIM
  Serial.begin(9600);  // Initiation connexion Série
  SIM900.print("AT+CMGF=1\r"); // communication en ASCII
  delay(100);
  //SIM900.print("AT+CNMI=2,2,0,0,0\r"); //prepare données
  delay(100);  
  sendSMS("Arduino OK : envoyer Etat pour demander l'etat, envoyer On pour allumer la pompe");
  sendSMSMax("Arduino OK : envoyer Etat pour demander l'etat, envoyer On pour allumer la pompe");
  sendSMSAlex("Arduino OK : envoyer Etat pour demander l'etat, envoyer On pour allumer la pompe");
  sendSMSDenis("Arduino OK : envoyer Etat pour demander l'etat, envoyer On pour allumer la pompe");
  sendSMS("Arduino OK : envoyer Etat pour demander l'etat, envoyer On pour allumer la pompe");

  Serial.println("Arduino OK");
}

void loop() { 
  
  while (SIM900.available() >0) {
    incoming_char=SIM900.read(); //Get the character from the cellular serial port
    msg += incoming_char;
  }
  SIM900.flush();

  bruit = checkState(); // vérifie le bruit ambiant (Haut ou Bas)
  //Serial.print("bruit : ");
  //Serial.print(bruit);
  //Serial.print("     etat  : ");
  //Serial.println(etatPompe);
  
  if (etatPompe == HIGH) {  // Si la pompe vient de s'éteindre
    while (bruit ==LOW) {
      //Serial.print("compteurLOW : ");
      //Serial.println(compteur);
      compteur += 1;
      bruit = checkState();
      if (compteur > 10){
        etatPompe = LOW;
        compteur = 0;
        sendSMS("O vezoune plus...");
        sendSMSAlex("O vezoune plus...");
        sendSMSDenis("O vezoune plus...");
        sendSMSMax("O vezoune plus...");

        Serial.println("pompe éteinte");
        break;
      }
    }
    compteur = 0;
  }

  if (etatPompe == LOW) {  // Si la pompe vient de s'allumer
    while (bruit == HIGH) {
      compteur += 1;
      //Serial.print("compteurHIGH : ");
      //Serial.println(compteur);
      bruit = checkState();
      if (compteur > 10){
        etatPompe = HIGH;
        compteur = 0;
        sendSMS("O vezoune !!");
        sendSMSAlex("O vezoune !!");
        sendSMSDenis("O vezoune !!");
        sendSMSMax("O vezoune !!");
        Serial.println("pompe allumée");
        break;
      }
    }
    compteur = 0;
  }

  // Vérifie message venant du SIM900
  if (msg != "") {
  /* Serial.println(msg);
    for (int i = 0; i <= msg.length(); i++) {
      Serial.print ( i );
      Serial.print ( "  :  ");
      Serial.print(msg[i]);
        if (msg[i] == NULL ){
          Serial.print("ha");
        }
      Serial.println();
      }*/

  //Vérifie arrivée d'un SMS
  for (int i = 0; i <= msg.length(); i++) {
    if (msg[i] == '+' && msg[i+1] == 'C' && msg[i+2] == 'M' && msg[i+3] == 'T' && msg[i+4] == ':') {
      Serial.println("sms recu");
    }
  }

  //Si le message est ON
  if ( (msg[msg.length()-4] == 'o' || msg[msg.length()-4] ==  'O') && (msg[msg.length()-3] == 'n' || msg[msg.length()-3] ==  'N')){
      Serial.println("Allumage");
      sendSMSAlex("allumage en cours");
      sendSMSMax("allumage en cours");
      sendSMSDenis("allumage en cours");
      sendSMS("allumage en cours");
      turnServo();
      delay(5000);
      etatPompe = checkState();
      if (etatPompe == HIGH){
        sendSMSAlex("O Vezoune bien !");
        sendSMSMax("O Vezoune bien !");
        sendSMSDenis("O Vezoune bien !");
        sendSMS("O Vezoune bien!");

      }
      int j = 0 ;
      while (etatPompe == LOW){
        turnServo();
        delay(5000);
        etatPompe = checkState();
        if (etatPompe == HIGH){
          sendSMSAlex("O Vezoune bien !");
          sendSMSMax("O Vezoune bien !");
          sendSMSDenis("O Vezoune bien !");
          sendSMS("O Vezoune bien!");
          break;
        }
        else {
          j += 1;
          if (j>=1){
            sendSMSAlex("O Vezoune toujours pas... Verifier l'etat et reessayer");
            sendSMSMax("O Vezoune toujours pas... Verifier l'etat et reessayer");
            sendSMSDenis("O Vezoune toujours pas... Verifier l'etat et reessayer");
            sendSMS("O Vezoune toujours pas... Verifier l'etat et reessayer");

            break;
          }
        }
      }  
  }
  
  //Si le message est "Etat"
  if ((msg[msg.length()-6] == 'E' || msg[msg.length()-6] ==  'e') && (msg[msg.length()-5] == 'T' || msg[msg.length()-5] ==  't')  && (msg[msg.length()-4] == 'a' || msg[msg.length()-4] ==  'A')  && (msg[msg.length()-3] == 't' || msg[msg.length()-4] ==  'T')){
      //Serial.print("envoi de l'etat : ");
      //Serial.println(etatPompe);
      if (etatPompe == 1) {
        sendSMS("etat demande : 1");
        sendSMSAlex("Etat demande : La pompe est allumee");
        sendSMSMax("Etat demande : La pompe est allumee");
        sendSMSDenis("Etat demande : La pompe est allumee");
      }
      else if (etatPompe == 0) {
        sendSMS("etat demande : 0");
        sendSMSAlex("Etat demande : La pompe est eteinte");
        sendSMSMax("Etat demande : La pompe est eteinte");
        sendSMSDenis("Etat demande : La pompe est eteinte");
      }
  }
  msg = ""; 
 }
 delay(500);
}

// Fonction de vérification de l'état de la pompe 
int checkState() {
  bool etatPompe;
  int tab[100];
  long int sum = 0;
  for (int i = 0; i < 100; i++) { // lecture de 100 valeurs
    tab[i] = analogRead(A0);
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
    etatPompe = 1;
  }
  else (etatPompe = 0);
  
  return etatPompe;
}

void sendSMS(char msg[]) {
  SIM900.println("AT + CMGS = \"0699227482\""); 
  delay(100);
  SIM900.println(msg); 
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(3000);
}

void sendSMSAlex(char msg[]) {
  SIM900.println("AT + CMGS = \"0611498921\""); 
  delay(100);
  SIM900.println(msg); 
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(3000);
}

void sendSMSMax(char msg[]) {
  SIM900.println("AT + CMGS = \"0614421056\""); 
  delay(100);
  SIM900.println(msg); 
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(3000);
}

void sendSMSDenis(char msg[]) {
  SIM900.println("AT + CMGS = \"0603017328\""); 
  delay(100);
  SIM900.println(msg); 
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(3000);
}

void turnServo() {
  myservo.write(90);
  delay(1000);
  myservo.write(0);
}
