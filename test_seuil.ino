void setup() {
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
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
  Serial.print(sum/100);
  Serial.print(" ");
  Serial.println(sumMean);
 }
