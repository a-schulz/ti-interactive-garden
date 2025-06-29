## Inhalt für die Abschlusspräsentation

Technikteil
- Einbau externer Stromversorgung -> LEDs brauchen recht viel Strom
- Mäßige Komplexität durch die vielen Kabel der Reader
- Reader müssen außerdem weit genug auseinander sein, weil sie sich sonst gegenseitig beim Lesen stören
- Probleme bei der Spannungsversorgung der Reader, 3.3V Wandler notwendig
- When sharing MISO (Master In Slave Out) and MOSI (Master Out Slave In) lines for multiple RFID readers, it's generally not recommended to connect more than two or three readers directly in parallel. Sharing these lines can lead to signal degradation and communication issues, especially with certain reader models. For more than a few readers, solutions like using a logic OR gate on the MISO line or multiplexers are necessary
- https://arduino.stackexchange.com/questions/18313/issue-sharing-miso-with-multiple-rc522-rfid-readers


- Implementierung von drei Spielmodi
	- Kombination mit der Natur (Wasser, (Sonne))
	- Kombination mit anderen Pflanzen
	- Kombination mit Natur und anderen Pflanzen

- Feedback über die LED Ringe
	- Pulsieren in rot, grün, weiß

Startanimation um die Kinder zu motivieren