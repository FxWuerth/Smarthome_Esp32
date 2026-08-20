#define FASTLED_RMT_BUILTIN_DRIVER true
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "RTClib.h"
#include <Wire.h>
#include <Preferences.h>

RTC_DS3231 rtc;

Preferences prefs;
const char* ssid = "Mein_Wlan";
const char* password = "12345678";

WebServer server(80);
bool sensorValue = false;
char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
int einschalten = 0;
int ausschalten = 0;

#define NUM_LEDS    10        
#define NUM_STRANG  10
#define BRIGHTNESS  50       // helligkeit (0–255)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_STRANG][NUM_LEDS];


#define NUM_FLAKES_STRANG  2   // 2 pro Strang  => 50 mA


struct Snowflake {
  float pos;       
  float speed;     
  CRGB color;      
};

Snowflake flakes[NUM_STRANG][NUM_FLAKES_STRANG];


void setup() {

  
  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds[0], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds[1], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, 13, COLOR_ORDER>(leds[2], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, 15, COLOR_ORDER>(leds[3], NUM_LEDS);
  FastLED.addLeds<LED_TYPE, 25, COLOR_ORDER>(leds[4], NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(esp_random());
  // Schneeflocken zufällig initialisieren
  for(int s = 0; s < NUM_STRANG;s++){
   for (int i = 0; i < NUM_FLAKES_STRANG; i++) {
    flakes[s][i].pos = random(NUM_LEDS);
    flakes[s][i].speed = random(5, 20) / 100.0;  // 0.05–0.20 LEDs pro Frame
    flakes[s][i].color = CRGB(random(180, 255), random(220, 255), 255);
   }
  }

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Wire.begin(21,22);
  prefs.begin("sensor", false);
  if(prefs.isKey("stron") && prefs.isKey("stroff")){
    einschalten = prefs.getInt("stron");
    ausschalten = prefs.getInt("stroff");
  }
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println(WiFi.localIP());
  Serial.println("Verbunden!");
  
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  if(!rtc.begin()){
   Serial.println("Finde keinen RTC");
   while(true);
}

  server.on("/set", HTTP_POST, [](){
    
    if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not found");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Empfangener Body: " + body);

  StaticJsonDocument<200> json;
  DeserializationError error = deserializeJson(json, body);

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  String stron = json["stron"];
  String stroff = json["stroff"];
  einschalten = stron.toInt();
  ausschalten = stroff.toInt();
  prefs.putInt("stron", stron.toInt());
  prefs.putInt("stroff", stroff.toInt());
  Serial.printf("Empfangen: %s | %s\n", stron.c_str(), stroff.c_str());

  server.send(200, "application/json", "{\"status\":\"ok\"}");

  });
  server.on("/value", []() {
    
    StaticJsonDocument<100> doc;
    doc["value"] = sensorValue;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  Serial.println(einschalten);
  Serial.println(ausschalten);
  server.begin();
}

void loop() {
  server.handleClient();
  DateTime jetzt = rtc.now();
  int zeitaktuell = jetzt.hour() *100 + jetzt.minute();
  if(zeitaktuell >= einschalten && zeitaktuell <= ausschalten){
      // Schneeflocken aktualisieren
  for(int s = 0; s< NUM_STRANG; s++){
    fadeToBlackBy(leds[s], NUM_LEDS, 30);
   for (int i = 0; i < NUM_FLAKES_STRANG; i++) {
    flakes[s][i].pos += flakes[s][i].speed;

    // Wenn Flocke unten ist → oben neu starten
     if (flakes[s][i].pos >= NUM_LEDS) {
      flakes[s][i].pos = 0;
      flakes[s][i].speed = random(7, 17) / 100.0;
      flakes[s][i].color = CRGB(random(180, 255), random(220, 255), 255);
     }


    leds[s][(int)flakes[s][i].pos] += flakes[s][i].color;
   }
  }

  FastLED.show();
  delay(40); 
     //lichterkette an:
     sensorValue = true;
  } else{
    FastLED.clear(true);
    sensorValue = false;
  }
}

