#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

// Crear instancias de los sensores
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  
  // Inicializar el sensor AHT20
  if (!aht.begin()) {
    Serial.println("No se encontró el AHT20. Verifique la conexión.");
    while (1);
  }
  Serial.println("AHT20 iniciado correctamente.");

  // Inicializar el sensor BMP280
  if (!bmp.begin(0x77)) { // Cambia la dirección I2C si es necesario
    Serial.println("No se encontró el BMP280. Verifique la conexión.");
    while (1);
  }
  Serial.println("BMP280 iniciado correctamente.");
}

void loop() {
  // Leer datos del sensor AHT20
  sensors_event_t humidity, temp_aht;
  aht.getEvent(&humidity, &temp_aht);
  Serial.print("Temperatura AHT20: ");
  Serial.print(temp_aht.temperature);
  Serial.println(" °C");
  
  Serial.print("Humedad: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %");

  // Leer datos del sensor BMP280
  Serial.print("Temperatura BMP280: ");
  Serial.print(bmp.readTemperature());
  Serial.println(" °C");

  Serial.print("Presión: ");
  Serial.print(bmp.readPressure() / 100.0F);
  Serial.println(" hPa");

  delay(2000); // Esperar 2 segundos antes de la siguiente lectura
}
