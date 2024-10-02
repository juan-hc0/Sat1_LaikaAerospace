#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <HardwareSerial.h> // Para la comunicación UART con GPS
#include <TinyGPS++.h>      // Librería para interpretar los datos del GPS

// Crear instancias de los sensores
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

// Crear instancia de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definir los pines de los LEDs, el buzzer, el botón y UART
int ledPin = 2;         // LED general
int mpuLedPin = 4;      // LED específico para MPU6050
int buttonPin = 15;     // Pin del botón
int buzzerPin = 13;     // Pin del buzzer en ESP32
#define RX_PIN 16
#define TX_PIN 17

bool displayMPU = false;
bool displayGPS = false;

#define SEALEVELPRESSURE_HPA (1013.25)

// Crear el objeto serial para UART
HardwareSerial mySerial(2); // Usar UART2 para la comunicación con el GPS

TinyGPSPlus gps; // Objeto de la librería GPS

void setup() {
  Serial.begin(115200);

  // Configurar los pines de los LEDs y el buzzer como salidas
  pinMode(ledPin, OUTPUT);
  pinMode(mpuLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Configurar el pin del botón como entrada
  pinMode(buttonPin, INPUT_PULLUP);

  // Hacer sonar el buzzer durante 1 segundo al inicio
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);

  // Inicializar el sensor AHT20
  if (!aht.begin()) {
    Serial.println("No se encontró el AHT20. Verifique la conexión.");
    while (1);
  }
  Serial.println("AHT20 iniciado correctamente.");

  // Inicializar el sensor BMP280
  if (!bmp.begin(0x77)) { 
    Serial.println("No se encontró el BMP280. Verifique la conexión.");
    while (1);
  }
  Serial.println("BMP280 iniciado correctamente.");

  // Inicializar el sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("No se encontró el MPU6050. Verifique la conexión.");
    mpuLedPin = -1; // Apagar el LED específico si el MPU6050 no está conectado
  } else {
    Serial.println("MPU6050 iniciado correctamente.");
  }

  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("No se encontró la pantalla OLED. Verifique la conexión.");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("OLED Iniciada!");
  display.display();
  delay(2000);

  // Inicializar comunicación serial para el GPS
  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  // Leer el estado del botón
  if (digitalRead(buttonPin) == LOW) {
    if (displayMPU) {
      displayMPU = false;
      displayGPS = true;
    } else if (displayGPS) {
      displayGPS = false;
    } else {
      displayMPU = true;
    }
    delay(200); // Debounce del botón
  }

  // Encender el LED general mientras se envían los datos
  digitalWrite(ledPin, HIGH);

  if (displayMPU) {
    // Leer datos del sensor MPU6050
    if (mpuLedPin != -1) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      // Parpadear el LED específico para MPU6050
      digitalWrite(mpuLedPin, HIGH);
      delay(100); // Encendido del LED por 100 ms
      digitalWrite(mpuLedPin, LOW);
      delay(100); // Apagado del LED por 100 ms

      // Mostrar datos en la pantalla OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 28); 
      display.println("MPU6050");
      display.print("Acel X: ");
      display.print(a.acceleration.x);
      display.println(" m/s^2");
      display.print("Acel Y: ");
      display.print(a.acceleration.y);
      display.println(" m/s^2");
      display.print("Acel Z: ");
      display.print(a.acceleration.z);
      display.println(" m/s^2");
      display.display();

      // Enviar datos al monitor serial
      Serial.println("Datos MPU6050:");
      Serial.print("Acel X: ");
      Serial.print(a.acceleration.x);
      Serial.println(" m/s^2");
      Serial.print("Acel Y: ");
      Serial.print(a.acceleration.y);
      Serial.println(" m/s^2");
      Serial.print("Acel Z: ");
      Serial.print(a.acceleration.z);
      Serial.println(" m/s^2");
    } else {
      // Si el MPU6050 no está conectado, mostrar un mensaje de error en la pantalla OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 28); 
      display.println("MPU6050 NO");
      display.println("CONEXION");
      display.display();
    }
  } else if (displayGPS) {
    // Leer los datos del GPS
    while (mySerial.available() > 0) {
      char c = mySerial.read();
      gps.encode(c); // Codifica los datos GPS

      if (gps.location.isUpdated()) {
        // Borrar pantalla y mostrar datos actualizados
        display.clearDisplay();
        display.setCursor(0, 0);

        display.println(F("GPS Data:"));
        display.setCursor(0, 10);
        
        // Mostrar latitud y longitud
        display.print(F("Lat: "));
        display.println(gps.location.lat(), 6);
        display.print(F("Lng: "));
        display.println(gps.location.lng(), 6);

        // Mostrar el número de satélites
        display.setCursor(0, 30);
        display.print(F("Sats: "));
        display.println(gps.satellites.value());

        // Mostrar la altitud
        display.setCursor(0, 40);
        display.print(F("Alt: "));
        display.print(gps.altitude.meters());
        display.println(F(" m"));

        // Mostrar velocidad
        display.setCursor(0, 50);
        display.print(F("Vel: "));
        display.print(gps.speed.kmph());
        display.println(F(" km/h"));

        // Actualizar la pantalla OLED
        display.display();
      }
    }
  } else {
    // Apagar el LED específico para MPU6050 cuando no se está mostrando información del MPU6050
    if (mpuLedPin != -1) {
      digitalWrite(mpuLedPin, LOW);
    }

    // Leer datos del sensor AHT20
    sensors_event_t humidity, temp_aht;
    aht.getEvent(&humidity, &temp_aht);

    // Leer altitud del BMP280
    float altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);

    // Mostrar datos en la pantalla OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 38); 
    display.print("Temp: ");
    display.print(temp_aht.temperature);
    display.println(" C");

    display.print("Hum: ");
    display.print(humidity.relative_humidity);
    display.println(" %");

    display.print("Alt: ");
    display.print(altitude);
    display.println(" m");

    display.display();

    // Enviar datos al monitor serial
    Serial.println("Datos AHT20 y BMP280:");
    Serial.print("Temperatura AHT20: ");
    Serial.print(temp_aht.temperature);
    Serial.println(" C");
    Serial.print("Humedad: ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");
    Serial.print("Altitud: ");
    Serial.print(altitude);
    Serial.println(" m");
  }

  // Apagar el LED general después de enviar los datos
  digitalWrite(ledPin, LOW);

  delay(2000); // Esperar 2 segundos antes de la siguiente lectura
}
