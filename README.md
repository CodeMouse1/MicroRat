# MicroRat: Eine Technische Dokumentation

<div align="center">
	<img src="https://media3.giphy.com/media/sbFY50AHtKuyYMWtKj/giphy.gif?cid=6c09b9523kuqddo3wfcj003jzzqmvzqegmzmrfjg5tf4ogdw&ep=v1_internal_gif_by_id&rid=giphy.gif&ct=s" width= "300">

<div align="left">

Dieses Repository wurde im Rahmen der Fächer Verteilte Systeme und Steuergeräteentwicklung erstellt, um eine technische Dokumentation für das Projekt **MicroRat** für zukünftige Studenten darzulegen. Ziel dieses Projektes ist eine Hardware zu entwickeln, die an einer Micromouse orientiert ist. 

Wenn der Begriff einer Micromouse unbekannt ist findet man unter diesem Link ein Erläuterungsvideo:
https://www.youtube.com/watch?v=ZMQbHMgK2rw
***

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

***
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

***
## Hardware
Diese bereitgestellte Hardware stellt eine vereinfachte Version einer Micromouse dar, wie sie heutzutage häufig anzutreffen ist. Grund dafür ist, dass heutige Micromäuse stark optimiert sind, wie erwähnt soll diese als eine Einstiegsplattform in die Welt der autnonomen Pfadfindung und Robotik dienen.

**Grundlegender Aufbau:**

- Umfeldsensorik **Front**
- Aktorik und Verarbeitung **Mitte**
- Schutz, Versorgung und weitere Anschluss-Pins **Hinterseite**

Diese ausgelegten Pins bestehen aus 5 GPIOs, sowohl als auch einer UART und I²C Schnittstelle. Somit haben zukünftige Studenten die Möglichkeit je nach Anforderung die MicroRat zu erweitern.
<div align="center">
![Microrat_Render2](/uploads/c9efd14c18da91d574844993bb5b45e3/Microrat_Render2.png)
<div align="left">
	


## Integrate with your tools

- [ ] [Set up project integrations](https://gitlab.bht-berlin.de/s88832/microrat-technical-documentation/-/settings/integrations)

## Collaborate with your team

- [ ] [Invite team members and collaborators](https://docs.gitlab.com/ee/user/project/members/)
- [ ] [Create a new merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html)
- [ ] [Automatically close issues from merge requests](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically)
- [ ] [Enable merge request approvals](https://docs.gitlab.com/ee/user/project/merge_requests/approvals/)
- [ ] [Set auto-merge](https://docs.gitlab.com/ee/user/project/merge_requests/merge_when_pipeline_succeeds.html)

## Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] [Get started with GitLab CI/CD](https://docs.gitlab.com/ee/ci/quick_start/index.html)
- [ ] [Analyze your code for known vulnerabilities with Static Application Security Testing(SAST)](https://docs.gitlab.com/ee/user/application_security/sast/)
- [ ] [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/ee/topics/autodevops/requirements.html)
- [ ] [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/ee/user/clusters/agent/)
- [ ] [Set up protected environments](https://docs.gitlab.com/ee/ci/environments/protected_environments.html)

***

# Editing this README

When you're ready to make this README your own, just edit this file and use the handy template below (or feel free to structure it however you want - this is just a starting point!). Thank you to [makeareadme.com](https://www.makeareadme.com/) for this template.

## Suggestions for a good README
Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

## Name
Choose a self-explaining name for your project.

## Description
Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.

## Badges
On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

## Visuals
Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

## Installation
Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

## Usage
Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Support
Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

## Roadmap
If you have ideas for releases in the future, it is a good idea to list them in the README.

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Show your appreciation to those who have contributed to the project.

## License
For open source projects, say how it is licensed.

## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
