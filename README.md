# SoftSkillsProject: Gruppe 24: Hydration Tracker
Dieser Sensor trackt den Wasserstand deiner Flasche und benachrichtigt dich, wenn du zu wenig Wasser getrunken hast.

![download](https://user-images.githubusercontent.com/107244022/179734226-7afa8535-7d08-4d5d-b7c7-89bda8b9afde.png)

## Beteiligte
- Tim Schröder
- Robin Visser
- Markus Osterloh
- Kevin Janetzki
- Maximilian Bosse

## Features
- Wasserstand einer Flasche wird in regelmäßigen Zeitabständen gemessen
- Website gibt Informationen über Wasserhaushalt und Flascheninhalt 
- Füllmenge der Flasche wird über LED-Streifen visualisiert
- Regelmäßige Aktualisierung durch Webserver
- Discord-Benachrichtigung: Habe ich genug / zu wenig getrunken?

## Weitere Voraussetzungen zur Nutzung
- internetfähiges Endgerät

## Hardware
- ESP-8266 (Wemos D1 mini board)
- Load-Cell with ADC (HX711)
- Boost-Converter
- BMS with charger
- 18650 Lion
- RGB-LEDs

## Bekannte Fehler und Mängel
- Gewichtsmessungen sind fehleranfällig 
- Keine Kalibrierbarkeit auf beliebige Flaschen 
- Keine garantierte Wasserfestigkeit  
- Manuelle Änderung der Verbindungsdaten bei Netzwerkwechsel notwendig

## Dokumentation
Die Dokumentation zu unserem Projekt findet ihr auf folgender Seite: 
- https://wp.uni-oldenburg.de/soft-skills-und-technische-kompetenz-wise20212022-projektgruppe-24/

## Beispielbilder 
- Website:

 ![download](https://user-images.githubusercontent.com/107244022/179706044-4b16d3e0-b1f5-40b4-be9b-cb097c7cc4c6.png)

- Discord Benachrichtigung:
Coming Soon
