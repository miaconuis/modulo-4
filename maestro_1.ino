/*
  📡 Comunicación Maestro-Esclavo con ESP32 usando ESP-NOW + ACK
  --------------------------------------------------------------
  - Este código es para el ESP32 MAESTRO.
  - Funciones principales:
    ✅ Recibe datos del Esclavo (Temp, Humedad, Estado del Buzzer)
    ✅ Envía confirmación ACK al Esclavo
    ✅ Permite enviar comandos ON/OFF al buzzer del Esclavo
*/

#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

// ==== MAC del Esclavo (STA del esclavo) ====
// Cambia esta dirección por la que tenga el ESP32 Esclavo
uint8_t slaveAddress[] = {0x24, 0x0A, 0xC4, 0xB2, 0xD5, 0x88};  

// ==== Estructura de datos recibidos del esclavo ====
typedef struct struct_message {
  float temperature;
  float humidity;
  uint8_t buzzerState;
} struct_message;

struct_message incomingData;   // Variable para almacenar lo recibido

// Estado del buzzer que controla el maestro
uint8_t buzzerCommand = 0;

// ==== Callback de recepción (cuando el maestro recibe datos del esclavo) ====
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, data, sizeof(incomingData));

    Serial.println("📥 Datos recibidos del Esclavo:");
    Serial.printf("   🌡️ Temp: %.1f°C\n", incomingData.temperature);
    Serial.printf("   💧 Humedad: %.1f%%\n", incomingData.humidity);
    Serial.printf("   🔔 Buzzer (en esclavo): %s\n", incomingData.buzzerState ? "ON" : "OFF");

    // Enviar ACK al Esclavo
    uint8_t ack = 1; // Valor 1 = ACK
    esp_now_send(slaveAddress, &ack, sizeof(ack));
    Serial.println("✅ ACK enviado al esclavo");
  }
}

// ==== Callback de envío (cuando el maestro envía algo) ====
void OnDataSent(const wifi_tx_info_t *wifi_tx_info, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("📤 Mensaje enviado correctamente");
  } else {
    Serial.println("❌ Error al enviar mensaje");
  }
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🚀 Iniciando ESP32 Maestro...");

  // Configurar WiFi en modo estación
  WiFi.mode(WIFI_STA);

  // Fijar canal (debe ser el mismo que el esclavo)
  int channel = 6;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error inicializando ESP-NOW");
    ESP.restart();
  }

  // Configurar peer esclavo
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, slaveAddress, 6);
  peerInfo.channel = channel;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  esp_now_del_peer(slaveAddress);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Error al agregar peer esclavo");
  }

  // Registrar callbacks
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  Serial.println("✅ Maestro listo con ESP-NOW + ACK");
}

// ==== Loop principal ====
void loop() {
  // Ejemplo: cambiar estado del buzzer manualmente
  // Cada 15 segundos alterna ON/OFF y se lo manda al esclavo
  static unsigned long lastCommandTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastCommandTime > 15000) {
    lastCommandTime = currentTime;

    buzzerCommand = !buzzerCommand; // Alternar estado
    esp_err_t result = esp_now_send(slaveAddress, &buzzerCommand, sizeof(buzzerCommand));

    if (result == ESP_OK) {
      Serial.printf("📤 Comando enviado al esclavo -> Buzzer %s\n", buzzerCommand ? "ON" : "OFF");
    } else {
      Serial.println("❌ Error al enviar comando al esclavo");
    }
  }

  delay(500);
}
