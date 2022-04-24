# Großprojekt Föhn

Für das Fritz-Felsenstein-Haus aus Königsbrunn wurde das bestehende Projekt **Föhn** erweitert und verbessert. 
Der Föhn ermöglicht Rollstuhlfahrern und gehbehinderte Personen und Kindern sich selbständig die Haare zu föhnen. 

![Föhn Übersicht](/Dokumentation/Abschlusspr%C3%A4sentation%20F%C3%B6hn%2021/Folie6.PNG)

## Projektübernahme 
Die Vorgängergruppe hat den Föhn mit einem Raspberry Pi gesteuert. Die lange Vorbereitungszeit bis das System bereit war, hat zu lange gedauert. \
Es konnte nicht eingesetzt werden, da es nicht den geltenden EMV-Bestimmungen entsprach. Die Software des Föhns hatte Fehler und konnte nach wenigen Minuten nicht wieder benutzt werden. 

## Projektziele

- Requirements Engineering um Anforderungen und Projektziel besser abzustimmen. 
- Lasten- und Pflichtenheft erstellen 
- Austausch der Steuerung durch ein uController, STM32F446RE um Vorbereitungszeit zu verringern 
- Sicherer störungsfreier und konstanter Föhnbetrieb
- Universelle HMI-Schnittstelle um Taster austauschen zu können
- Drehbewegung des Föhns um gleichmäßiges Föhnen zu ermöglichen und Brandstellen zu vermeiden
- Bessere Sensorik verwenden 

## Software

Die Initialisierung wurde die CubeMx generiert. Die Software wurde mithilfe des STM HAL geschrieben. 

Die Software wurde mithilfe von UML Diagrammen dokumentiert. 
Eines der wichtigsten Diagramme zeigt den Programmablaufplan. ![UML Diagramm Programmablaufplan](/Dokumentation/Abschlusspr%C3%A4sentation%20F%C3%B6hn%2021/Folie10.PNG)


## Motoransteuerung 

Eine Anforderung der Leitung war einen DC-Motor zu verwenden. Es wurde eine Mechanik und auch eine elektrische sowie Software Ansteuerung entwickelt. ![DC-Motor](/Dokumentation/Abschlusspr%C3%A4sentation%20F%C3%B6hn%2021/Folie13.PNG)
Die EMV Werte lagen über den erlaubten Grenzwerten. Es wurde eine Lösung mithilfe eines Servo-Motors gewählt. Die EMV-Messwerte lagen danach unterhalb der Grenzwerte. 
![Servo-Motor](/Dokumentation/Abschlusspr%C3%A4sentation%20F%C3%B6hn%2021/Folie7.PNG)

## Zeitungsartikel
Auch in der Zeitung wurde über das Projekt und den Föhn berichtet. 

![Föhn Anwendung](/Dokumentation/Foehn_Anwendung.png)

[Staz Zeitungsartikel](https://www.staz.de/region/koenigsbrunn/lokales/tastern-mikrocomputern-studenten-entwickeln-spiele-alltagsgegenstaende-fuer-koerperbehinderte-id231747.html)

