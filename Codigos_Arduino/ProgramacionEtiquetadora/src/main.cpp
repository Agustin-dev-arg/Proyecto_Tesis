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

// Declaración de pines
const int Pin_Start = 16;
const int Pin_Stop = 17;

// Sensor Ultrasonido ED_Sensor_US1
const int ED_Sensor_US1_ECHO = 18;
const int ED_Sensor_US1_TRIG = 19;

// Salidas a relé
const int SD_Rele_Cinta = 21;
const int SD_Rele_Etiquetadora = 22;

// Entrada digital sensor de etiqueta
const int ED_Sensor_De_Etiqueta = 23;

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
  // Estado inicial de las salidas de relé
  pinMode(Pin_Start, INPUT);
  pinMode(Pin_Stop, INPUT);

  // Configuración de pines para el sensor ultrasonido
  pinMode(ED_Sensor_US1_ECHO, INPUT);
  pinMode(ED_Sensor_US1_TRIG, OUTPUT);

  // Configuración de salidas a relé
  pinMode(SD_Rele_Cinta, OUTPUT);
  pinMode(SD_Rele_Etiquetadora, OUTPUT);

  // Estado inicial de las salidas de relé
  digitalWrite(SD_Rele_Cinta, LOW);         // Cambia a HIGH si tu relé es activo en alto
  digitalWrite(SD_Rele_Etiquetadora, LOW);  // Cambia a HIGH si tu relé es activo en alto

  // Configuración de entrada digital sensor de etiqueta
  pinMode(ED_Sensor_De_Etiqueta, INPUT);
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
  // --- Lógica de preparación: buscar GAP antes de etiquetar ---
  static bool preparado = false;
  static String ultimo_estado = "stop";
  // Detectar cambio de estado desde la web o botones físicos
  String estado_actual = "";
  if (ahora - lastConsulta >= intervaloConsulta) {
    lastConsulta = ahora;
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("http://192.168.1.7:8000/obtener_estado/");
      int httpCode = http.GET();
      if (httpCode == 200) {
        String payload = http.getString();
        int keyIndex = payload.indexOf("\"estado\"");
        if (keyIndex != -1) {
          int colonIndex = payload.indexOf(":", keyIndex);
          int quoteStart = payload.indexOf('"', colonIndex + 1);
          int quoteEnd = payload.indexOf('"', quoteStart + 1);
          if (quoteStart != -1 && quoteEnd != -1) {
            estado_actual = payload.substring(quoteStart + 1, quoteEnd);
          }
        }
        Serial.print("Estado actual: ");
        Serial.println(estado_actual);
      } else {
        Serial.print("Error consultando estado: ");
        Serial.println(httpCode);
      }
      http.end();
    }
  }

  // Si el usuario presiona STOP desde la web o botón físico, reiniciar preparación
  //Si ambas condiciones se cumplen, significa que el usuario acaba de presionar STOP (no que ya estaba en STOP)
  if (estado_actual == "stop" && ultimo_estado != "stop") {
    preparado = false;
    Serial.println("Preparación reiniciada por STOP");
    // Avisar al backend que entra en modo preparación
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("[DEBUG] Enviando preparacion=true al backend");
      HTTPClient http;
      http.begin("http://192.168.1.7:8000/actualizar_preparacion/?preparacion=true");
      int httpCode = http.GET();
      Serial.print("[DEBUG] Respuesta preparacion=true: ");
      Serial.println(httpCode);
      http.end();
    }
  }
  ultimo_estado = estado_actual;

  // Solo ejecutar la lógica de preparación si el estado es 'start' y no está preparado
  if (!preparado && estado_actual == "start") {
    digitalWrite(SD_Rele_Etiquetadora, HIGH);
    if (digitalRead(ED_Sensor_De_Etiqueta) == LOW) {
      digitalWrite(SD_Rele_Etiquetadora, LOW); // Detiene el motor
      Serial.println("Preparación lista: GAP detectado");
      preparado = true;
      // Avisar al backend que sale del modo preparación
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[DEBUG] Enviando preparacion=false al backend");
        HTTPClient http;
        http.begin("http://192.168.1.7:8000/actualizar_preparacion/?preparacion=false");
        int httpCode = http.GET();
        Serial.print("[DEBUG] Respuesta preparacion=false: ");
        Serial.println(httpCode);
        http.end();
      }
    }
    // NO return aquí: seguir ejecutando el resto del loop para que los botones físicos respondan
  } else if (estado_actual == "stop") {
    // Si está en stop, asegurarse de que el motor esté apagado
    digitalWrite(SD_Rele_Etiquetadora, LOW);
  }

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
        // Sincronizar con Django (enviar estado 'start')
        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin("http://192.168.1.7:8000/actualizar_estado/?accion=start");
          int httpCode = http.GET();
          Serial.print("Respuesta Django START: ");
          Serial.println(httpCode);
          http.end();
        }
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
        // Sincronizar con Django (enviar estado 'stop')
        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin("http://192.168.1.7:8000/actualizar_estado/?accion=stop");
          int httpCode = http.GET();
          Serial.print("Respuesta Django STOP: ");
          Serial.println(httpCode);
          http.end();
        }
      }
    }
  }
  lastButtonStateStop = readingStop;

  // ...otras tareas sin bloqueo...
}
