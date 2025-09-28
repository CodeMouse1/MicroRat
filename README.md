# MicroRat: Eine technische Dokumentation

<div align="center">
	<img src="https://media3.giphy.com/media/sbFY50AHtKuyYMWtKj/giphy.gif?cid=6c09b9523kuqddo3wfcj003jzzqmvzqegmzmrfjg5tf4ogdw&ep=v1_internal_gif_by_id&rid=giphy.gif&ct=s" width= "300">

<div align="left">

Dieses Repository dokumentiert und bündelt das Projekt MicroRat – eine an der Micromouse orientierte Plattform für Studierenden der BHT.
Es stellt ein vollständiges Paket aus Hardware und Software-Komponenten bereit und soll als Grundlage für eigene Projekte, Experimente und Weiterentwicklungen dienen.

Wenn der Begriff einer Micromouse unbekannt ist findet man unter diesem Link ein Erläuterungsvideo:
https://www.youtube.com/watch?v=ZMQbHMgK2rw

<div align="center">
	<img src="https://image.futurezone.at/images/cfs_square_1232/7995559/micromouselabyrinthss.jpg" width= "280">
	<img src="https://i.ytimg.com/vi/LAYdXIREK2I/maxresdefault.jpg" width= "500">
<div align="left">
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
	<img src="https://www.infineon.com/export/sites/default/media/products/Microcontrollers/devel_tools_SW/DAvE3.jpg_45004101.jpg" width= "50">
<div align="left">

##### Was ist DAVE IDE?
DAVE IDE (Digital Application Virtual Engineering Integrated Development Environment) ist eine leistungsstarke und benutzerfreundliche Entwicklungsumgebung, die speziell für die Entwicklung von Software für Mikrocontroller und eingebettete Systeme entwickelt wurde. DAVE IDE unterstützt eine Vielzahl von Mikrocontrollern und bietet eine integrierte Plattform für die Programmierung, das Debugging und die Analyse von Embedded-Anwendungen.

**Hauptfunktionen von DAVE IDE**

1. **Projektverwaltung:**

DAVE IDE ermöglicht die einfache Erstellung und Verwaltung von Projekten. Wenn Ihr auf _File_ oben rechts klicken könnt ihr neue Projekte erstellen, bestehende Projekte importieren und eure Projekte in verschiedenen Konfigurationen verwalten.

2. **Code-Editor:**

Der integrierte Code-Editor unterstützt Syntax-Highlighting, Auto-Vervollständigung und Fehlererkennung. Dies erleichtert das Schreiben und Verwalten von Code erheblich.

3. **Debugging-Tools:**

Die integrierten Debugging-Tools ermöglichen es euch, euren Code in Echtzeit zu testen und Fehler zu beheben. Ihr könnt Breakpoints setzen, den Code Schritt für Schritt durchlaufen und die Werte von Variablen überwachen. Um ihren Code zu debuggen einfach auf den 🪲 drücken.

##### Erste Schritte mit DAVE IDE
**Installation:**

Ladet die neueste Version von DAVE IDE von der offiziellen Website herunter und folgen Sie den Installationsanweisungen. Hier der Link: [⬇️](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.daveide)

**Erstellen eines neuen Projekts:**

Startet DAVE IDE und wählt "Neues Projekt erstellen" aus. Folgt dem Assistenten, um ein Projekt für eure spezifischen Mikrocontroller zu konfigurieren. Hier muss der richtige Microcontroller ausgewählt werden.

**Code schreiben:**

Nutzen Sie den integrierten Code-Editor, um euren Anwendungscode zu schreiben. Sobald das erledigt ist muss man die nötigen APPs dem Prozessor zuweisen, dazu _Manual Pin Allocator_ klicken und anschließend auf _Generate Code_ drücken bevor ihr den Code auf _Build Code_ drückt.
Hier ein Bsp Code für blinkende LED:
```
#include "DAVE.h"           //Declarations from DAVE Code Generation (includes SFR declaration)

#define ONESEC 1000000U     //definiere 1 sec

uint8_t Timer_1s;	    //Timer 1s


void Toggle_LED(void){
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_LED);	//Toggle LED alle 1s
}

int main(void)
{

	DAVE_Init();           /* Initialization of DAVE APPs  */
	
	Timer_1s = SYSTIMER_CreateTimer(ONESEC,SYSTIMER_MODE_PERIODIC,(void*)Toggle_LED,NULL);	//Init Timer 1s
	SYSTIMER_StartTimer(Timer_1s);	//Starte Timer 1s

}
```

**Debuggen und Testen:**

Verwenden Sie die Debugging-Tools, um euren Code zu testen. Setzt Breakpoints, um den Code Schritt für Schritt zu durchlaufen und die Funktionalität zu überprüfen.

**Implementierung auf Hardware:**

Sobald der Code fehlerfrei ist, könnt ihr ihn auf euren Mikrocontroller hochladen und die Anwendung auf der tatsächlichen Hardware testen.

**Tipps und Best Practices:**

- Beginnt mit der APP Zuordnung und falls sich bestimmte APPs die gleiche CCU brauchen ist es besser alle auf die selbe zu packen statt einer neuen Instanz zu erstellen
- Sobald Ihr neue Apps hinzufügt immer auf _Generate Code_ drücken
- Wenn manche APPs unklar sind bietet die APP Help/APP Info umfangreiche Hilfe von Überblick zu Beispiel Code
- Persöhnliche Microcontrollerempfehlung XMC 4700 RelaxKit

Hier noch 2 Lins zu hilfreichen Dokumenten:
- [Introduction](https://www.infineon.com/dgdl/Infineon-DAVE_Introduction-DT-v01_00-EN.pdf?fileId=5546d462636cc8fb01645f681d4713ed)
- [Quick Start](https://www.infineon.com/dgdl/DAVE+Quick+Start+PresentationTutorial+v2-1.pdf?fileId=db3a304344d727a80144d99976b2002b)

### KiCad

<div align="center">
	<img src="https://avatars.githubusercontent.com/u/3374914?s=280&v=4" width= "50">
<div align="left">

##### Warum KiCAD?

Für dieses Projekt haben wir uns entschieden, KiCAD als unser EDA-Tool (Electronic Design Automation) zu verwenden. KiCAD bietet eine Vielzahl von Vorteilen, die es zu einer ausgezeichneten Wahl für die Entwicklung von Leiterplatten (PCBs) machen:

##### Open-Source und Kostenfrei
KiCAD ist ein Open-Source-Tool, das kostenfrei verfügbar ist. Dies ermöglicht es jedem, das Programm ohne finanzielle Hürden zu nutzen und von einer großen Community unterstützt zu werden.

##### Leistungsstarke Funktionen
KiCAD bietet eine umfangreiche Palette an Funktionen, die den gesamten PCB-Design-Prozess abdecken:
- **Schematic Capture**: Leistungsfähige und benutzerfreundliche Tools zur Erstellung und Bearbeitung von Schaltplänen.
- **PCB Layout**: Umfassende Layout-Tools, die komplexe Designs ermöglichen, einschließlich Unterstützung für mehrere Lagen und hochauflösende Darstellungen.
- **3D Viewer**: Ein integrierter 3D-Viewer, der es ermöglicht, das fertige PCB-Design in einer dreidimensionalen Ansicht zu betrachten und zu überprüfen.

##### Bibliotheken und Bauteildatenbank
KiCAD verfügt über eine umfangreiche Bibliothek von Bauteilen und Symbolen, die ständig aktualisiert und erweitert wird. Zusätzlich können Benutzer eigene Bauteile und Symbole erstellen und hinzufügen. Für die Erstellung eigener Symbole und Footprints hilft dieses Video:[🎥](https://www.youtube.com/watch?v=1V23NaExcg0) 

##### Flexibilität und Anpassbarkeit
Durch die Open-Source-Natur von KiCAD ist das Tool sehr flexibel und anpassbar. Benutzer können Plugins und Skripte erstellen, um den Funktionsumfang zu erweitern und an ihre spezifischen Bedürfnisse anzupassen.

##### Plattformübergreifend
KiCAD ist plattformübergreifend verfügbar und läuft auf Windows, macOS und Linux. Dies ermöglicht eine konsistente Benutzererfahrung unabhängig vom Betriebssystem.

### Unterstützung durch eine aktive Community
KiCAD wird von einer aktiven und engagierten Community unterstützt. Dies bedeutet, dass es eine Fülle von Ressourcen, Tutorials und Foren gibt, in denen Benutzer Hilfe und Unterstützung finden können.

### Regelmäßige Updates
Das Entwicklerteam hinter KiCAD veröffentlicht regelmäßig Updates und neue Versionen, die Fehlerbehebungen und neue Funktionen beinhalten. Dies gewährleistet, dass das Tool stets auf dem neuesten Stand der Technik bleibt.

Weitere Informationen zu KiCAD findest du auf der [offiziellen Webseite](https://www.kicad.org/).

### Fusion 360

<div align="center">
	<img src="https://damassets.autodesk.net/content/dam/autodesk/www/product-imagery/badge-75x75/simplified-badges/fusion-360-product-design-extension-2023-simplified-badge-75x75.png" width= "50">
<div align="left">

##### Was ist Fusion 360?

Fusion 360 ist eine cloudbasierte 3D-CAD-, CAM- und CAE-Software, die von Autodesk entwickelt wurde. Sie bietet umfassende Werkzeuge für das Entwerfen, Testen und Fertigen von Produkten. Die Software ermöglicht parametrische Modellierung, direkte Modellierung, Freiformmodellierung und Netzbearbeitung, wodurch Benutzer präzise und flexible Designs erstellen können. Darüber hinaus bietet Fusion 360 Funktionen für Simulationen und Generative Design, um die Leistung von Produkten zu testen und innovative Lösungen zu finden. Dank der Cloud-Integration können Teams in Echtzeit zusammenarbeiten und Daten sicher speichern.

#### Hauptfunktionen von Fusion 360

1. **3D-CAD-Modellierung:**

Parametrische Modellierung: Erstellung von präzisen, regelbasierten Modellen.
Direkte Modellierung: Bearbeiten und Anpassen von Geometrien ohne Historie.
Freiformmodellierung: Erstellen und Bearbeiten von organischen und komplexen Formen.
Netzbearbeitung: Arbeiten mit 3D-Scans und Mesh-Daten.

2. **CAM (Computer-Aided Manufacturing):**

Werkzeugpfad-Generierung: Erstellung von Werkzeugwegen für CNC-Bearbeitungen.
2.5D-, 3D-, 4- und 5-Achsen-Bearbeitung: Unterstützung verschiedener Bearbeitungsprozesse.
Bearbeitungssimulation: Virtuelles Testen der Bearbeitungsprozesse zur Fehlervermeidung.

3. **CAE (Computer-Aided Engineering):**

Simulationen: Durchführung von Finite-Elemente-Analysen (FEA) für strukturelle, thermische und modale Analysen.
Generatives Design: Automatisiertes Erzeugen von Design-Alternativen basierend auf definierten Anforderungen und Einschränkungen.

4. **Zusammenarbeit und Datenmanagement:**

Cloud-Integration: Echtzeit-Zusammenarbeit und Datenspeicherung in der Cloud.
Versionskontrolle: Verwalten von Designänderungen und Versionsverläufen.

5. **Rendering und Animation:**

Fotorealistisches Rendering: Erstellen von hochwertigen Bildern und Animationen zur Visualisierung von Designs.
Animationswerkzeuge: Erstellen von Bewegungssimulationen und interaktiven Präsentationen.

##### Erste Schritte mit Fusion 360

**Installation:**

Ein Guide zum Installieren der Fusion 360 Umgebung finden Sie im folgenden Link: [⬇️](https://www.autodesk.de/support/technical/article/caas/sfdcarticles/sfdcarticles/DEU/How-does-Fusion-360-get-installed.html)

**Erstellen einer neuen Konstruktion:**

Um eine neue Konstruktion zu erstellen, kann über der oberen Leiste Rechts das Plussymbol angeklickt werden. Alle Werkzeuge zum erstellen eines 3D-Objektes befinden unterhalb der oberen Leiste.

**Export:**

Wenn das erstellte Modell fertig ist, kann man über den Reiter "Dienstprogramme" eine Datei für z.B. einen 3D-Drucker erstellt werden. 

**Hilfreiche Links**

Hier sind Hilfreiche Links, um einen ausführlichen Einblick in Fusion 360 zu bekommen: 

- [Fundamentals](https://help.autodesk.com/view/fusion360/ENU/courses/)
- [Guides](https://help.autodesk.com/view/fusion360/ENU/?guid=GUID-670346CA-4CF8-4009-9E9B-09FCC6803B61)

### JLCPCB

**Was ist JLCPCB?**

JLCPCB ist ein führender chinesischer Anbieter von Leiterplatten (Printed Circuit Boards, PCBs) und elektronischen Dienstleistungen. Das Unternehmen bietet eine breite Palette von Dienstleistungen und Produkten für die Herstellung von Leiterplatten an. Hier sind einige der Hauptmerkmale und Dienstleistungen von JLCPCB:

1. **Leiterplattenherstellung:**

Prototypen und Massenproduktion: Herstellung von kleinen und großen Mengen an PCBs.
Verschiedene PCB-Typen: Ein- und mehrschichtige Leiterplatten, flexible PCBs, starr-flexible PCBs und Aluminium-PCBs.
Schnelle Lieferzeiten: Express-Service für schnelle Produktion und Lieferung.

2. **PCB-Design und Anpassung:**

Kundenspezifische Designs: Anpassung der PCBs an spezifische Anforderungen und Spezifikationen.
Design for Manufacturing (DFM) Unterstützung: Hilfe bei der Optimierung von Designs für die Produktion.

3. **Zusatzleistungen:**

SMD-Stencil: Bereitstellung von Schablonen für die SMD-Bestückung.
Bauteilbeschaffung: Unterstützung bei der Beschaffung von elektronischen Bauteilen.
PCB-Montage: Komplettmontage der Leiterplatten einschließlich Bestückung der Bauteile.

4. **Benutzerfreundliche Online-Plattform:**

Einfacher Bestellprozess: Benutzerfreundliche Online-Bestellplattform mit Preisangeboten in Echtzeit.
Datei-Uploads: Unterstützung für das Hochladen von Gerber-Dateien und anderen Design-Daten.
Online-Tracking: Verfolgung des Produktionsstatus und der Lieferung in Echtzeit.

5. **Qualitätskontrolle:**

Strenge Inspektionen: Umfassende Qualitätskontrollprozesse, einschließlich elektrischer Tests und optischer Inspektionen.
Zertifizierungen: Erfüllung internationaler Qualitätsstandards und Zertifizierungen wie ISO 9001.

JLCPCB ist bekannt für seine kostengünstigen und zuverlässigen Dienstleistungen und wird häufig von Hobbyisten, Start-ups und etablierten Unternehmen in der Elektronikindustrie genutzt.

**Unser Anwendungsfall**

Wir haben diesen Service genutzt, um günstig unsere Platinen drucken zu lassen.  

**Links**

- [Guide](https://jlcpcb.com/blog/183-the-ultimate-guide-to-pcba)

## Beispielanwendung

Der hier dargestellte Code ist im ZIP Ordner zu finden und soll ledeglich einen kurzen Überblick verschaffen.
```
#include "DAVE.h"
#include "Dummy.h"

int button_status = 0;


int main(void)
{
	DAVE_Init();           /* Initialization of DAVE APPs  */
	//Dummycode_Init();

	while(true){
	button_status = DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
		if(button_status == 1){
			Dummycode_Init();
			button_status = 0;
		}
	}
}
```
Die vorbereitete .main ist so aufgebaut das sobald der auf der MicroRat zu findende Knopf gedrückt wird unser Dummycode, auch Demo Programm genannt, automatisch abgespielt und liest alles Sensoren via UART aus. Als nächstes betrachten wir unsere vorbereitete Dummy.c File wo der eigentliche Funktionscode zu finden ist.
```
#include "Dave.h"
#include "Dummy.h"
#include <stdio.h>          // Für sprintf

#define PERIODIC_READ 100000U

uint8_t UART_String[100];   // Init APPs und Array
uint32_t Timer_100ms;       // -||-
uint32_t Capture_t;         // -||-

XMC_VADC_RESULT_SIZE_t ADC_Wert_IR_L;   // Variablen zur  Speicherung der ADC-Ergebnisse
XMC_VADC_RESULT_SIZE_t ADC_Wert_IR_R;

float captured_time_us;     // Variable für den Ultraschallsensor
float distanz_ultra;        // -||-
int IR_L, IR_R;

void Sensoren_Auslesen_100ms(void){ //Alle 100ms aufgerufen
	//IR
	ADC_Wert_IR_R = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A);  // Holt neuen ADC Wert des IR Sensors
	ADC_Wert_IR_L = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_B);  // -||-
	IR_R = ADC_Wert_IR_R;
	IR_L = ADC_Wert_IR_L;
	//ULTRASCHALL
	CAPTURE_GetCapturedTime(&CAPTURE_ULTRA, &Capture_t);    // Berechnung der Distanz mehr Info dazu in der Doku
	captured_time_us = ((float)Capture_t * 333.33)/1000;
	distanz_ultra = captured_time_us /58;   
	//DREZAHL

	//UART-Transmit
	sprintf((char*)UART_String,	" Ultraschall: %.2fcm   // Formatierung der Sensordaten in eine Zeichenkette
        IR_R: %d IR_L: %d\n\r", distanz_ultra, IR_R, IR_L);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));

	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);    // LED blinken
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);    // -||-

}

void Dummycode_Init(void)
{
	Timer_100ms = SYSTIMER_CreateTimer(PERIODIC_READ,SYSTIMER_MODE_PERIODIC,(void*)Sensoren_Auslesen_100ms,NULL);   // Deklaration des periodischen Timers
	SYSTIMER_StartTimer(Timer_100ms);   // Start
}

```
**Mazegenerator**
<div align="center">
	<img src="https://codebox.net/assets/images/maze-generator/maze_square.svg" width= "50">
<div align="left">
Für eine einfache und direkte Erstellung eines Labyrinthes, empfehlen wir diese Webseite: [MazeGenerator](https://codebox.net/pages/maze-generator/online)

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

## Kontaktinformationen
Bei Fragen oder Problemen könnt Ihr uns gerne kontaktieren:

Entwickler: [Marcus Stake Alvarado](mailto:s88832@bht-berlin.de)

Entwickler: [Nico Hinrichs](mailto:s87913@bht-berlin.de)

<div align="center">

**Viel Spaß mit der MicroRat**

![Face-14-512](/uploads/a4284d4abe20ee8daba8ffba1f62310e/Face-14-512.jpg)



