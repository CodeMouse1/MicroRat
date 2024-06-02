# MicroRat: Eine technische Dokumentation

<div align="center">
	<img src="https://media3.giphy.com/media/sbFY50AHtKuyYMWtKj/giphy.gif?cid=6c09b9523kuqddo3wfcj003jzzqmvzqegmzmrfjg5tf4ogdw&ep=v1_internal_gif_by_id&rid=giphy.gif&ct=s" width= "300">

<div align="left">

Dieses Repository wurde im Rahmen der Fächer Verteilte Systeme und Steuergeräteentwicklung erstellt, um eine technische Dokumentation für das Projekt **MicroRat** für zukünftige Studenten darzulegen. Ziel dieses Projektes ist eine Hardware zu entwickeln, die an einer Micromouse orientiert ist. 

Wenn der Begriff einer Micromouse unbekannt ist findet man unter diesem Link ein Erläuterungsvideo:
https://www.youtube.com/watch?v=ZMQbHMgK2rw


## Technische Beschreibung
Micromouse ist ein Robotikwettbewerb, in dem sich kleine, autonome Roboterfahrzeuge in einem Labyrinth zurechtfinden müssen. Die Micromouse-Roboterfahrzeuge sind vollständig autonom. Sie bewegen sich ohne Hilfe von außen durch das Labyrinth. Es ist dem Entwickler freigestellt, wie das Fahrzeug die Wände des Labyrinthes erkennt; verbreitet sind vorn am Fahrzeug angebrachte und schräg nach außen ausgerichtete Infrarot-Abstandssensoren. Unsere Micromouse soll als Einstieg in diese Thematik dienen.

**Unsere Umsetzung**

Eine Micromouse sieht je nach Erbauer unterschiedlich aus. Unser Projekt soll grundlegende Funktionen erhalten, sodass die Benutzung und Programmierung für den User einfach ist. Für die Abstandserfassung sind Infrarot-Sensoren von SHARP jeweils in einem Winkel von 45° befestigt, sowie ein Ultraschall-Sensor an der Spitze montiert. Dadurch erfasst die Micromouse die genaue Position der Wände.

An den beiden Motoren, welche zur Fortbewegung dienen, ist zudem ein Encoder für die Drehzahlerfassung befestigt. 

Für eine Reibungslose Stromversorgung benutzen wir ein selbsterstelltes 2s1p Akkupack aus Li-Ion Zellen.

Angesteuert wird das Kit über ein XMC-1402, der über ein Debug-Connector programmierbar ist. Unsere **MicroRat** kann die erfassten Daten der Sensoren dem User über eine serielle-Schnittstelle bereitstellen. Eine eigene Implementierung eines Pfadfindungsalgorithmus ist im Rahmen dieses Projektes nicht vorgesehen.  

<div align="center">
	<img src="https://image.futurezone.at/images/cfs_square_1232/7995559/micromouselabyrinthss.jpg" width= "300">
<div align="center" > Dieses Bild stammt aus [futurezone](https://futurezone.at/digital-life/roboter-maus-micromouse-labyrinth-kuenstliche-intelligenz-nerf-gun-kurios-videos/402506761)


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


## Hardware
Diese bereitgestellte Hardware stellt eine vereinfachte Version einer Micromouse dar, wie sie heutzutage häufig anzutreffen ist. Grund dafür ist, dass heutige Micromäuse stark optimiert sind, wie erwähnt soll diese als eine Einstiegsplattform in die Welt der autnonomen Pfadfindung und Robotik dienen.

**Grundlegender Aufbau:**

- Umfeldsensorik **Front**
- Aktorik und Verarbeitung **Mitte**
- Schutz, Versorgung und weitere Anschluss-Pins **Hinterseite**

Diese ausgelegten Pins bestehen aus 5 GPIOs, sowohl als auch einer UART und I²C Schnittstelle. Somit haben zukünftige Studenten die Möglichkeit je nach Anforderung die MicroRat zu erweitern.
<div align="center">

<p float="center">
  <img src="/uploads/bf74777127c349fa86007dc492ed4637/_removal.ai__8c2d7579-b76e-41dc-b2b4-d0dd401914db-microrat3_DNO46E.png" width="600" />
  <img src="/uploads/52044ddf1023aa9465557396d21156f6/Microrat5-removebg.png" width="600" /> 
</p>
<div align="left">

## Anleitung?

## Beispielanwendung

## Lizenzinformationen
Dieses Projekt ist zur allgemeinen Nutzung freigegeben. Es gibt keine spezifische Lizenz. Ihr könnt es nutzen, wie ihr möchtet.

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

## Kontaktinformationen
Bei Fragen oder Problemen könnt Ihr uns gerne kontaktieren:

Entwickler: [Marcus Stake Alvarado](mailto:s88832@bht-berlin.de)

Entwickler: [Nico Hinrichs](mailto:s87913@bht-berlin.de)

<div align="center">

**Viel Spaß mit der MicroRat**

![Face-14-512](/uploads/a4284d4abe20ee8daba8ffba1f62310e/Face-14-512.jpg)



