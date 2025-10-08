# 🚀 MÓDULO 4: Comunicación ESP32 con ESP-NOW y MQTT 📡

Este repositorio contiene los recursos y códigos desarrollados para el **Módulo 4**, orientado al estudio y aplicación de **protocolos de comunicación inalámbrica** entre dispositivos **ESP32**, combinando **ESP-NOW** para la comunicación local y **MQTT** para la integración con **Node-RED**.

---

## 🧩 Estructura general del módulo

El módulo está dividido en tres etapas progresivas, donde se integran conceptos de comunicación, control y visualización de datos:

### 🔹 Etapa 1: Preparación del entorno con Docker y Node-RED
- Configuración del entorno de trabajo mediante **Docker** y **Docker Compose**.  
- Ejecución de **Node-RED** y del **broker MQTT (Mosquitto)** dentro de contenedores.  
- Verificación del correcto funcionamiento de los servicios mediante comandos básicos.

### 🔹 Etapa 2: Comunicación Maestro–Esclavo con ESP-NOW
- Implementación de una red local entre dos **ESP32**: un **maestro** y un **esclavo**.  
- Comunicación directa mediante **ESP-NOW**, utilizando direcciones **MAC** como identificadores únicos.  
- Envío y recepción de datos de sensores (DHT11) y control de actuadores (buzzer).  
- Sincronización de canal y confirmación **ACK** para garantizar confiabilidad.

### 🔹 Etapa 3: Integración con MQTT y Node-RED
- Conexión del **ESP32 maestro** a una red WiFi y al **broker MQTT**.  
- Publicación y suscripción a *topics* para el intercambio de datos en la nube.  
- Recepción, procesamiento y visualización de variables en tiempo real mediante **Node-RED Dashboard**.  
- Implementación de bloques lógicos en Node-RED para la organización y validación de los mensajes.

---

## ⚙️ Archivos principales

| Archivo / Carpeta | Descripción |
|--------------------|-------------|
| `docker-compose.yml` | Define los servicios de Docker: Node-RED y Mosquitto. |
| `mac.ino` | Código para obtener la dirección MAC única de cada ESP32. |
| `esclavo.ino` | Programa del ESP32 esclavo: sensado y transmisión de datos vía ESP-NOW. |
| `maestro.ino` | Programa del ESP32 maestro: recepción de datos y envío de comandos. |
| `Maestro-node-red.ino` | Versión del maestro con conexión WiFi y comunicación MQTT hacia Node-RED. |

---

## 🎯 Objetivo del módulo

Desarrollar un sistema de comunicación inalámbrica completo que combine **ESP-NOW (punto a punto)** y **MQTT (cliente-servidor)**, comprendiendo su funcionamiento, configuración e integración en un entorno práctico de **Internet de las Cosas (IoT)**.

---

## 🧠 Tecnologías empleadas

- 🐳 **Docker** y **Docker Compose**
- 🔴 **Node-RED**
- 🛰️ **MQTT (Mosquitto Broker)**
- ⚡ **ESP32 y protocolo ESP-NOW**
- 🌐 **WiFi e integración IoT**

---

## 👨‍💻 Autores y créditos

Proyecto desarrollado por el **Grupo de Trabajo de Grado MIACON**  
en colaboración con el **Grupo de Investigación CEMOS – Universidad Industrial de Santander (UIS)**.

**Autores:**  
- Juan Tibaduiza  
- Manuel Solano  

📍 Bucaramanga, Colombia — 2025

---

### ✨ “De la comunicación local al Internet de las Cosas, paso a paso.” 🌐
