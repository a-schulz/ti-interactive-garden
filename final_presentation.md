# Interaktiver Garten - Abschlusspräsentation

## 1. Projektübersicht (30 Sek.)
- Interaktiver Lernspielzeug für Grundschulkinder der ersten und zweiten Klasse
- Konzept: Kinder platzieren Gemüse-Objekte mit RFID-Tags auf dem Spielbrett
- Ziel: Spielerisches Lernen über Pflanzenbeziehungen und optimale Wachstumsbedingungen
- Feedback durch farbige LED-Ringe: rot (schlecht), grün (gut), weiß (neutral)

## 2. Technische Umsetzung (90 Sek.)
### Hardware-Aufbau
- Arduino UNO als Steuereinheit
- 6 RFID-RC522 Reader zur Erkennung der Pflanzenobjekte
- 2 Ketten mit je 4 WS2812B LED-Ringen (12 LEDs pro Ring) für visuelles Feedback
- Externe 5V-Stromversorgung für ausreichende Stromversorgung der LEDs

### Technische Herausforderungen
- Bedarf an externer Stromversorgung wegen hohem Stromverbrauch der LED-Ringe
- Komplexe Verkabelung durch die Vielzahl an RFID-Readern
- RFID-Reader müssen ausreichend voneinander entfernt sein, um gegenseitige Störungen zu vermeiden
- 3,3V Spannungswandler für die RFID-Reader notwendig
- Probleme beim parallelen Betrieb mehrerer RFID-Reader:
  * Gemeinsame Nutzung von MISO/MOSI-Leitungen führt zu Signaldegradation
  * Bei mehr als 2-3 Readern werden spezielle Lösungen wie Logik-OR-Gatter oder Multiplexer notwendig

## 3. Softwarekonzept (60 Sek.)
### Pflanzendatenbank
- Implementierung verschiedener Gemüsesorten mit individuellen Eigenschaften:
  * Tomaten, Kartoffeln, Karotten, etc.
  * Jede Pflanze hat bevorzugte und tolerierte Umgebungsbedingungen
  * Definierte Beziehungen zwischen Pflanzenarten (mag/neutral/mag nicht)

### Drei Spielmodi
1. **Umgebungsmodus**: Überprüft, ob die Pflanze mit den Umgebungsfaktoren (Wasser, Sonne) gedeiht
2. **Nachbarschaftsmodus**: Überprüft Kompatibilität mit benachbarten Pflanzen
3. **Kombinierter Modus**: Berücksichtigt sowohl Umgebung als auch Nachbarn

### Visuelle Effekte
- Feedback über die LED-Ringe durch verschiedene Farbmuster:
  * Pulsierendes Rot: ungünstige Bedingungen
  * Pulsierendes Grün: optimale Bedingungen
  * Pulsierendes Weiß: neutrale/tolerierte Bedingungen
- Spezielle Startanimation zur Motivation der Kinder

## 4. Pädagogischer Wert (30 Sek.)
- Vermittlung von Grundkenntnissen über Pflanzenbau und Gartenbau
- Förderung des Verständnisses für ökologische Zusammenhänge
- Haptisches Lernerlebnis durch physische Interaktion mit den Pflanzenobjekten
- Spielerische Entdeckung von Pflanzenkompatibilitäten und Umweltfaktoren

## 5. Fazit & Ausblick (30 Sek.)
- Erfolgreiche Implementierung eines interaktiven Lernsystems für Kinder
- Potenzial für Erweiterungen:
  * Mehr Pflanzenarten und komplexere Beziehungen
  * Zusätzliche Umgebungsfaktoren wie pH-Wert oder Nährstoffe
  * Integration von Soundeffekten für multimodales Feedback
- Mögliche Anwendung in Grundschulen oder Umweltbildungszentren