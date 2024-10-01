#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Crear instancia de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección I2C 0x3C
    Serial.println("No se encontró la pantalla OLED. Verifique la conexión.");
    while (1);
  }
  
  // Encender todos los píxeles de la pantalla
  display.clearDisplay();   // Limpiar la pantalla
  display.fillScreen(WHITE); // Encender todos los píxeles
  display.display();        // Actualizar la pantalla
}

void loop() {
  // No se necesita nada en el loop para esta prueba
}
