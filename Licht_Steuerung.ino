#include <WiFi.h>
#include <WebServer.h>  


const char* ssid = "Mein_Wlan";
const char* password = "12345678";
String einschalten;
String ausschalten;

LED_PIN 8;
WebServer server(80);


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarte ESP32 Webserver...");
  pinMode(LED_PIN, output);

  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nVerbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/led", handleLed);
  server.onNotFound(handleNotFound);

  
  server.begin();
  Serial.println("Webserver gestartet!");
}

void loop() {
  
  server.handleClient();
}


void handleRoot() {
  server.send(200, "text/plain", "ESP32 ist online. Verwende /data oder /led");
}


void handleData() {
  if (server.hasArg("msg")) {
    String msg = server.arg("msg");
    Serial.println("Empfangen über /data: " + msg);
    server.send(200, "text/plain", "Empfangen: " + msg);
    ausschalten = msg.substring(4,8);
   einschalten = msg.substring(0,4);
  } else {
    server.send(400, "text/plain", "Fehler: kein 'msg' Parameter!");
  }
}


void handleLed() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    Serial.println("LED-Befehl: " + state);

    if (state == "on") {
      digitalWrite(LED_PIN, HIGH);
      server.send(200, "text/plain", "LED eingeschaltet");
    } else if (state == "off") {
      digitalWrite(LED_PIN, LOW);
      server.send(200, "text/plain", "LED ausgeschaltet");
    } else {
      server.send(400, "text/plain", "Unbekannter LED-Status");
    }
  } else {
    server.send(400, "text/plain", "Fehler: kein 'state' Parameter!");
  }
}


void handleNotFound() {
  server.send(404, "text/plain", "Seite nicht gefunden!");
}



