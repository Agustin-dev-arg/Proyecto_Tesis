#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

const char *ssid = "Personal-410";
const char *password = "xRKkJYf5UM";

HTTPClient http;
AsyncWebServer server(80);

//Declaracion de pines
const int Pin_Start = 16;
const int Pin_Stop = 17;

// Variables para antirrebote START
unsigned long lastDebounceTimeStart = 0;
const unsigned long debounceDelay = 30; // ms
int lastButtonStateStart = HIGH;
int buttonStateStart = HIGH;

// Variables para antirrebote STOP
unsigned long lastDebounceTimeStop = 0;
int lastButtonStateStop = HIGH;
int buttonStateStop = HIGH;

void setup() {
  pinMode(Pin_Start, INPUT);
  pinMode(Pin_Stop, INPUT);
  Serial.begin(9600);
  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }
  Serial.println("Conexión exitosa a la red WiFi");

  // Imprimir la dirección IP asignada
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());

  String peticion = "http://192.168.1.7:8000/recibir_mensaje/?mensaje=" + WiFi.localIP().toString();

  Serial.println(peticion);

  http.begin(peticion.c_str());
  http.addHeader("Host", "192.168.1.7");

  int httpCode = http.GET();

  Serial.println("Respuesta");
  Serial.println(httpCode);
  http.end();

}

unsigned long lastConsulta = 0;
const unsigned long intervaloConsulta = 1000; // 1 segundo

void loop() {
  unsigned long ahora = millis();
  // Consulta periódica al servidor
  if (ahora - lastConsulta >= intervaloConsulta) {
    lastConsulta = ahora;
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("http://192.168.1.7:8000/obtener_estado/");
      int httpCode = http.GET();
      if (httpCode == 200) {
        String payload = http.getString();
        // Parsear el JSON para extraer solo el valor de 'estado'
        String estado = "";
        int keyIndex = payload.indexOf("\"estado\"");
        if (keyIndex != -1) {
          int colonIndex = payload.indexOf(":", keyIndex);
          int quoteStart = payload.indexOf('"', colonIndex + 1);
          int quoteEnd = payload.indexOf('"', quoteStart + 1);
          if (quoteStart != -1 && quoteEnd != -1) {
            estado = payload.substring(quoteStart + 1, quoteEnd);
          }
        }
        Serial.print("Estado actual: ");
        Serial.println(estado); // Solo muestra 'start' o 'stop'
      } else {
        Serial.print("Error consultando estado: ");
        Serial.println(httpCode);
      }
      http.end();
    }
  }

  // --- Antirrebote y lectura de botón START ---
  int readingStart = digitalRead(Pin_Start);
  if (readingStart != lastButtonStateStart) {
    lastDebounceTimeStart = ahora;
  }
  if ((ahora - lastDebounceTimeStart) > debounceDelay) {
    if (readingStart != buttonStateStart) {
      buttonStateStart = readingStart;
      if (buttonStateStart == LOW) { // Asumiendo botón activo en LOW
        Serial.println("Botón START presionado");
        // Aquí va la lógica para sincronizar con Django (enviar estado 'start')
      }
    }
  }
  lastButtonStateStart = readingStart;

  // --- Antirrebote y lectura de botón STOP ---
  int readingStop = digitalRead(Pin_Stop);
  if (readingStop != lastButtonStateStop) {
    lastDebounceTimeStop = ahora;
  }
  if ((ahora - lastDebounceTimeStop) > debounceDelay) {
    if (readingStop != buttonStateStop) {
      buttonStateStop = readingStop;
      if (buttonStateStop == LOW) { // Asumiendo botón activo en LOW
        Serial.println("Botón STOP presionado");
        // Aquí va la lógica para sincronizar con Django (enviar estado 'stop')
      }
    }
  }
  lastButtonStateStop = readingStop;

  // ...otras tareas sin bloqueo...
}
