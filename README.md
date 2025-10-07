# MicroRat: Eine technische Dokumentation

<div align="center">
	<img src="https://media3.giphy.com/media/sbFY50AHtKuyYMWtKj/giphy.gif?cid=6c09b9523kuqddo3wfcj003jzzqmvzqegmzmrfjg5tf4ogdw&ep=v1_internal_gif_by_id&rid=giphy.gif&ct=s" width= "300">

<div align="left">

Dieses Repository dokumentiert und bündelt das Projekt MicroRat – eine an der Micromouse orientierte Plattform für Studierenden der BHT.
Es stellt ein vollständiges Paket aus Hardware und Software-Komponenten bereit und soll als Grundlage für eigene Projekte, Experimente und Weiterentwicklungen dienen.
<div align="center">
	<img src="Bilder & Videos/Videos/VideoGIF.gif" width= "800">
<div align="left">
<br>
Wenn der Begriff einer Micromouse unbekannt ist findet man unter diesem Link ein Erläuterungsvideo:
[▶️](https://www.youtube.com/watch?v=ZMQbHMgK2rw)
<br>

## Technische Beschreibung
Micromouse ist ein internationaler Robotik-Wettbewerb, bei dem kleine, autonome Roboter ein Labyrinth selbstständig erkunden und den schnellsten Weg zum Ziel finden müssen.
Die Fahrzeuge agieren vollkommen ohne externe Hilfe und entscheiden selbstständig, wie sie Hindernisse und Wände erkennen.

_Die MicroRat soll als Einstieg in diese Thematik dienen._

**Hardware Umsetzung**

Das Aussehen einer Micromouse hängt stark vom jeweiligen Erbauer ab. Die MicroRat ist bewusst so gestaltet, dass sie grundlegende Funktionen mit einfacher Benutzbarkeit und Programmierbarkeit kombiniert.
Zur Umgebungserfassung kommen folgende Sensoren zum Einsatz:
###### Sensorik
- SHARP Infrarot-Sensoren, jeweils im 90°-Winkel senkrecht zu den Wänden montiert
- HC-SR04 Ultraschall-Sensor an der Vorderseite

###### Antrieb
- DC-Motoren mit Encodern für Antrieb und Drehzahlerfassung 

###### Stromversorgung
- 2s1p Li-Ion Akkupack für stabile Energieversorgung  

###### Steuerung & Schnittstellen

- XMC-1402 Mikrocontroller, programmierbar über Debug-Connector
- UART Schnittstelle zur Ausgabe der Sensordaten 

<div align="center">

<p float="center">
  <img src="Bilder & Videos/Bilder/Microrat1.5.png" width="400" />
  <img src="Bilder & Videos/Bilder/Microrat1.5Top.png" width="400" /> 
</p>
Hier ein Link zu einer 3D Ansicht: [3D-Modell](https://a360.co/3VB2RBQ)
<div align="left">
<br>

**Software-Architektur**

Die MicroRat wird über den XMC-1402 Mikrocontroller gesteuert, der Sensoren ausliest, Motoren ansteuert und die Labyrinthnavigation übernimmt.

###### Entwicklungsumgebung
- **Sprache:** C
- **IDE:** DAVE™ 
- **Debugging:** Über den Debug-Connector des XMC4500

###### Hauptfunktionen
- **Sensordaten-Auswertung:** IR- und Ultraschallsensor erkennen Wände und Hindernisse
- **Motorsteuerung:** Drehzahlgeregelte Ansteuerung der DC-Motoren mittels PD-Regler
- **Labyrinth-Algorithmus:**
	- Wandfolger (links/rechts), kartografiert
	- Floodfill, Optimierung des schnellsten Weges zum Ziel
- **Kommunikation:** Sensordaten über UART für Debugging & Logging
<div align="center">

<p float="center">
  <img src="Bilder & Videos/Videos/mms_simu.gif" width="600" />
</p>
<div align="left">


## Verwendung von KiCad 8.0
<div align="center">
	<img src="https://avatars.githubusercontent.com/u/3374914?s=280&v=4" width= "50">
<div align="left">
Für dieses Projekt haben wir KiCad 8.0 gewählt aus folgenden Gründen:
<br/><br/>

- **Fortschrittliche Funktionen:** Bietet erweiterte Tools für Schaltplanerstellung und Leiterplattendesign.
- **Open-Source-Software:** Kostenfrei und kontinuierlich verbessert durch eine große Community.
- **Plattformübergreifende Unterstützung:** Ermöglicht nahtlose Zusammenarbeit unabhängig vom Betriebssystem.

- **Kompatibilität und Exportmöglichkeiten:** Unterstützt viele Dateiformate und erleichtert die Integration mit anderen Tools.

Für eine grundlegende Einführung in KiCad bitte dieser Ratte folgen: [🐀](https://docs.kicad.org/)

## Anleitung

### Dave IDE 
<div align="center">
	<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSpdBPyPUTOO0MjXxbu3xy34bQi7HSOZktbZg&s" width= "50">
<div align="left">
<br>

DAVE IDE (Digital Application Virtual Engineering) ist eine benutzerfreundliche Entwicklungsumgebung für Mikrocontroller und eingebettete Systeme. Sie bietet Programmierung, Debugging und Analyse in einer Plattform.

###### Hauptfunktionen:

- **Projektverwaltung:** Projekte erstellen, importieren und konfigurieren.
- **Code-Editor:** Syntax-Highlighting, App-Help und Fehlererkennung.
- **Debugging:** Breakpoints setzen, Schritt-für-Schritt-Debugging, Variablen überwachen.

##### Erste Schritte mit DAVE IDE
- **Installation:**[⬇️](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.daveide)
- **Projekt importieren:** Datei → Importieren → ZIP auswählen.

### Micromouse Simulator
<div align="center">
	<img src="https://cdn-icons-png.freepik.com/512/8346/8346064.png" width= "50">
<div align="left">
<br>
Zum einfachen Testen und Erproben von Micromouse-Software ohne physische Hardware wird die Nutzung des MMS-Simulators empfohlen, mit dem Labyrinthe virtuell erstellt und Algorithmen sicher ausprobiert werden können: [Micromouse Simulator](https://github.com/mackorone/mms)

## Lizenzinformationen
Dieses Projekt ist Lizensiert unter der CC 4.0 Lizenz
https://creativecommons.org/licenses/by/4.0/

## Mitwirken
Wir freuen uns auf eure Erweiterungen! Wenn du an diesem Projekt mitarbeiten möchtest, folge bitte diesen Schritten:

1. **Fork dieses Repository**: Klicke oben rechts auf die Schaltfläche "Fork", um eine Kopie des Repositories in deinem GitLab-Konto zu erstellen.

2. **Clone das Forked Repository**: Klone dein geforktes Repository auf deinen lokalen Rechner.
    ```bash
    git clone https://github.com/dein-username/dein-repository.git
    ```

3. **Erstelle einen neuen Branch**: Erstelle einen neuen Branch für deine Arbeit.
    ```bash
    git checkout -b mein-neuer-branch
    ```

4. **Nimm deine Änderungen vor**: Führe die gewünschten Änderungen durch und committe sie.
    ```bash
    git add .
    git commit -m "Beschreibe deine Änderungen"
    ```

5. **Push deine Änderungen**: Push deine Änderungen zu deinem Fork.
    ```bash
    git push origin mein-neuer-branch
    ```

6. **Erstelle einen Pull Request**: Gehe zu deinem geforkten Repository auf GitLab und klicke auf die Schaltfläche "New Pull Request", um deine Änderungen zur Überprüfung einzureichen.

## Entwickler

Hardware & Software: Marcus Stake Alvarado

Ursprüngliches Hardware-Design: Nico Hinrichs

Ansprechprofessor: Prof. Dipl.-Ing. Koshan Mahdi

<div align="center">

**Viel Spaß mit der MicroRat**

<img src="/Bilder & Videos/Bilder/Face-14-512.jpg" width= "100">



