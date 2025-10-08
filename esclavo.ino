/*
  📡 Comunicación Maestro-Esclavo con ESP32 usando ESP-NOW + ACK
  --------------------------------------------------------------
  - Este código es para el ESP32 ESCLAVO.
  - Funciones principales:
    ✅ Lee temperatura y humedad del sensor DHT11
    ✅ Envía los datos al ESP32 Maestro vía ESP-NOW
    ✅ Recibe comandos del Maestro (para encender/apagar buzzer)
    ✅ Implementa reintentos hasta recibir ACK de confirmación
*/

#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include "DHT.h"

// ==== Configuración de pines y sensor ====
#define DHTPIN 4            // Pin donde conectamos el DHT11
#define DHTTYPE DHT11       // Tipo de sensor DHT
#define BUZZER_PIN 23       // Pin de salida para el buzzer

DHT dht(DHTPIN, DHTTYPE);   // Objeto del sensor DHT11

// ==== MAC del Maestro (copiar la MAC del ESP32 Maestro en modo STA) ====
uint8_t masterAddress[] = {0xCC, 0xDB, 0xA7, 0x30, 0x2E, 0x20}; 

// ==== Estructura de datos para enviar ====
typedef struct struct_message {
  float temperature;   // Temperatura leída
  float humidity;      // Humedad leída
  uint8_t buzzerState; // Estado actual del buzzer (ON/OFF)
} struct_message;

struct_message dataToSend;   // Variable que guarda lo que se va a enviar
uint8_t buzzerState = 0;     // Estado inicial del buzzer (apagado)

// ==== Variables de control de tiempo ====
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // Cada 10 segundos se envían datos

// ==== Configuración de ACK ====
const unsigned long ackTimeout = 200; // Tiempo máximo de espera por ACK (ms)
bool ackReceived = false;             // Bandera que indica si llegó ACK

// ==== Callback de recepción (cuando el esclavo recibe datos del maestro) ====
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if(len == sizeof(uint8_t)) {
    // El maestro envía un byte con el estado del buzzer
    uint8_t receivedValue = *incomingData;
    buzzerState = receivedValue;

    // Control físico del buzzer
    digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);

    Serial.printf("💡 Comando recibido del Maestro: Buzzer %s\n",
                  buzzerState ? "ON" : "OFF");
  }
}

// ==== Callback de envío (cuando el esclavo intenta enviar algo) ====
void OnDataSent(const wifi_tx_info_t *wifi_tx_info, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("📤 Datos enviados correctamente");
  } else {
    Serial.println("❌ Error al enviar datos");
  }
}

// ==== Callback para recepción de ACK (confirmación del maestro) ====
void OnAckRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if(len == sizeof(uint8_t) && incomingData[0] == 1) { // Si recibo "1" como ACK
    ackReceived = true;
    Serial.println("✅ ACK recibido del maestro");
  }
}

// ==== Setup principal ====
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🚀 Iniciando ESP32 Esclavo...");

  // Inicializar sensor y buzzer
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Configurar WiFi en modo estación
  WiFi.mode(WIFI_STA);

  // Forzar canal fijo (ambos ESP32 deben usar el mismo canal)
  int channel = 6;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error inicializando ESP-NOW");
    ESP.restart(); // Reinicia si falla
  }

  // Configuración del Maestro como "peer"
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = channel;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  // Eliminar peer anterior y agregar el nuevo
  esp_now_del_peer(masterAddress);
  if(esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Error al agregar peer maestro");
  }

  // Registrar callbacks
  esp_now_register_recv_cb(OnDataRecv); // Para recibir comandos
  esp_now_register_send_cb(OnDataSent); // Para confirmar envío
  esp_now_register_recv_cb(OnAckRecv);  // Para recibir ACK

  Serial.println("✅ Esclavo listo con ESP-NOW + ACK");
}

// ==== Bucle principal ====
void loop() {
  unsigned long currentTime = millis();

  // Solo enviamos cada cierto intervalo
  if(currentTime - lastSendTime >= sendInterval) {
    lastSendTime = currentTime;

    // Lectura del sensor DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if(!isnan(h) && !isnan(t)) {
      // Llenar estructura de datos
      dataToSend.temperature = t;
      dataToSend.humidity = h;
      dataToSend.buzzerState = buzzerState;

      ackReceived = false; // Reiniciamos bandera de ACK
      int retries = 0;
      const int maxRetries = 5;

      // Intentamos enviar hasta recibir ACK
      while(!ackReceived && retries < maxRetries) {
        esp_err_t result = esp_now_send(masterAddress,
                                        (uint8_t*)&dataToSend,
                                        sizeof(dataToSend));

        if(result != ESP_OK) Serial.println("❌ Error enviando datos");

        // Esperamos un poco por el ACK
        unsigned long startWait = millis();
        while(!ackReceived && millis() - startWait < ackTimeout) {
          delay(5); // Espera activa
        }

        // Si no hay ACK, reintentamos
        if(!ackReceived) {
          retries++;
          Serial.printf("⚠️ No se recibió ACK, reintentando (%d/%d)\n",
                        retries, maxRetries);
        }
      }

      // Resultado final del envío
      if(ackReceived) {
        Serial.printf("📤 Datos enviados -> Temp: %.1f°C | Hum: %.1f%% | Buzzer: %s\n",
                      t, h, buzzerState ? "ON" : "OFF");
      } else {
        Serial.println("❌ No se pudo enviar, ACK no recibido después de varios intentos");
      }
    } else {
      Serial.println("⚠️ Error leyendo DHT11");
    }
  }

  delay(1000); // Pausa para no saturar
}
