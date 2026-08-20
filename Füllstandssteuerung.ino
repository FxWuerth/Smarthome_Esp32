#include <WiFi.h>
#include <WebServer.h> 
#include <NewPing.h>

const char* ssid = "Mein_Wlan";        
const char* password = "12345678"; 

#define tp 5     
#define ep 18    
#define md 400   //maximale distanz
long liter;
NewPing sonar(tp, ep, md);
WebServer server(80);  


void handleRoot() {
  server.send(200, "text/html", "<h1>ESP32 ist online</h1>");
}

void handleValue() {
  float volumen = liter;  
  String json = "{\"temp\": " + String(volumen, 2) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Verbunden! IP-Adresse: ");
  Serial.println(WiFi.localIP());

  
  server.on("/", handleRoot);       
  server.on("/value", handleValue); 

  server.begin();  
  Serial.println("Webserver gestartet");
}

void loop() {
  server.handleClient(); 
   delay(50);
  unsigned int distance = sonar.ping_cm();
  liter = (130-distance)*4.2;
}
