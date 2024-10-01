// Definir el pin al que está conectado el LED
int ledPin = 2;

void setup() {
  // Configurar el pin del LED como salida
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Encender el LED
  digitalWrite(ledPin, HIGH);
  delay(1000); // Esperar 1 segundo
  
  // Apagar el LED
  digitalWrite(ledPin, LOW);
  delay(1000); // Esperar 1 segundo
}
