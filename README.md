# printserver-esp8266

* This project allows you to use an ESP8266 as a Wi-Fi print server.
* At the moment it just listens on a TCP port and forwards the data received from it to the printer. This protocol is known as "Socket" or "HP JetDirect". IPP support is in developement.
* If a new connection arrives while a print job is being processed, the new job is stored in the SPIFFS filesystem (must fit in ~3MB, otherwise it's discarded due to lack of space) and printed as soon as the printer is ready.
* It's mainly aimed at parallel port printers, which can be connected in two different ways:
	* Directly (uses 10 GPIO pins - one for BUSY, one for STROBE and 8 for the data lines)
	* Using a shift register, which reduces the amount of required pins to 5 (BUSY, STROBE, and 3 to drive the shift register to which the data lines are connected; currently tested with a 74HC595)
* Experimental support for serial printers (not tested)

## Useful links
* Socket/JetDirect protocol: http://lprng.sourceforge.net/LPRng-Reference-Multipart/socketapi.htm
* Parallel port:
	* Wikipedia: https://en.wikipedia.org/wiki/Parallel_port
	* Full specification (IEEE 1284): http://ieeexplore.ieee.org/document/469123/
	* Arduino forum thread: https://forum.arduino.cc/index.php?topic=74776.0
