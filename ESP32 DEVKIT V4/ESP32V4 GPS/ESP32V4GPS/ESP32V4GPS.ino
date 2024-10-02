#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <HardwareSerial.h> // Para la comunicación UART con GPS
#include <TinyGPS++.h>      // Librería para interpretar los datos del GPS

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definir los pines UART
#define RX_PIN 16
#define TX_PIN 17

// Crear el objeto serial para UART
HardwareSerial mySerial(2); // Usar UART2 para la comunicación con el GPS

TinyGPSPlus gps; // Objeto de la librería GPS

void setup() {
  // Inicializar comunicación serial para el GPS
  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se encontró la pantalla OLED"));
    for (;;);
  }

  // Mensaje de inicio
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Esperando GPS..."));
  display.display();
}

void loop() {
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
}
