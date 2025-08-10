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

void setup() {

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

void loop() {
  // put your main code here, to run repeatedly:
  
}
