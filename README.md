# ESP32 Smart-Home Module

Drei ESP32-Sketches für ein selbstgebautes Smart-Home-System. Jeder ESP32 hängt
im WLAN und stellt einen HTTP-Webserver auf Port 80 bereit. Eine selbst
geschriebene Flutter-App ist der Client: sie liest Werte aus oder setzt
Einstellungen.

```
Flutter-App ──HTTP──► ESP32 (Webserver :80) ──► Sensor / LEDs / Relais
            ◄─JSON──
```

## Licht_Steuerung/Licht_Steuerung.ino

Einfache Lichtsteuerung über WLAN. Endpunkte:

- `/led?state=on|off` — schaltet den LED-Pin
- `/data?msg=HHMMHHMM` — nimmt Ein-/Ausschaltzeit entgegen und zerlegt sie


## Steuerung_Schneeflocken

LED-Weihnachtsdeko mit Schneeflocken-Animation, zeitgesteuert über die App.

- **FastLED**: 5 Stränge à 10 WS2812B-LEDs, je zwei „Flocken" (`struct Snowflake`
  mit Position, Tempo, Farbe) laufen nach unten; `fadeToBlackBy()` macht den Schweif
- **DS3231**: liefert die Uhrzeit für das Ein-/Ausschaltfenster
- **Preferences**: speichert die Zeiten im Flash, überlebt einen Stromausfall
- `/set` (POST, `{"stron":1700,"stroff":2300}`) setzt die Zeiten,
  `/value` (GET) meldet zurück, ob die Beleuchtung gerade läuft

## Füllstandssteuerung

Füllstand einem Regenwasserbehälter per Ultraschallsensor HC-SR04 (Bibliothek NewPing).
Der Sensor misst von oben die Distanz zur Wasseroberfläche, daraus wird das
Volumen berechnet: `liter = (130 - distance) * 4.2` (130 cm Behälterhöhe,
4,2 Liter pro cm). `/value` liefert den Wert als JSON an die App.

## Hardware & Bibliotheken

ESP32 DevKit · DS3231 (Pins 21/22) · WS2812B · HC-SR04 · Relais-Modul
`WiFi`, `WebServer`, `ArduinoJson`, `FastLED`, `RTClib`, `NewPing`, `Preferences`

WLAN-Zugangsdaten stehen als Platzhalter im Code und müssen vor dem Flashen
angepasst werden.

