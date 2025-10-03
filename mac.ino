#include <WiFi.h>       // Librería para manejar WiFi en ESP32
#include "esp_wifi.h"   // Librería de bajo nivel para obtener la MAC

/*
   Objetivo:
   Este programa obtiene la dirección MAC del ESP32 y la imprime
   en un formato que se pueda copiar y pegar directamente en otro código C,
   por ejemplo: {0xE8, 0x6B, 0xEA, 0xDE, 0xB0, 0xE0};
*/

void setup() {
  // Inicializa el puerto serie para ver resultados en el Monitor Serie
  Serial.begin(115200);

  // Pone al ESP32 en modo estación (como cuando se conecta a un router WiFi)
  WiFi.mode(WIFI_STA);

  // Creamos un arreglo de 6 posiciones donde se guardará la MAC
  uint8_t mac[6];

  // Obtiene la MAC del ESP32 en modo estación (STA)
  esp_wifi_get_mac(WIFI_IF_STA, mac);

  /*
     Ahora imprimimos la MAC con el siguiente formato:
     {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
     Este formato es útil porque se puede copiar y pegar en otro código
     como un arreglo de tipo uint8_t.
  */

  Serial.print("{");   // Abre la llave
  for (int i = 0; i < 6; i++) {
    // Imprime cada byte en hexadecimal con el prefijo 0x
    Serial.printf("0x%02X", mac[i]);

    // Agrega coma y espacio entre los valores, excepto al final
    if (i < 5) Serial.print(", ");
  }
  Serial.println("};");   // Cierra la llave y agrega punto y coma
}

void loop() {
  // Volvemos a leer la MAC (aunque realmente no cambia)
  uint8_t mac[6];
  esp_wifi_get_mac(WIFI_IF_STA, mac);

  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.printf("0x%02X", mac[i]);
    if (i < 5) Serial.print(", ");
  }
  Serial.println("};");

  delay(2000); // Espera 2 segundos antes de imprimir otra vez
}

