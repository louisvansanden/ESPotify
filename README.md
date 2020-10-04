# ESPotify
#### An Arduino based module to use with the ESP8266 that allows the communication with the Spotify API via HTTP.
------------------------------------------
## Functionality
This module is able to execute following Spotify tasks:
- play / pause
- previous / next
- refresh the access token
- play from a specific playlist

It's possible to interact with the ESP8266 in two ways:
- via a __WebServer__ created by the ESP8266
- via __IR-sensor__ using a custom protocol

## Webserver
The IP-address is printed on the serial monitor in the setup. To interact with the ESP __surf to the given IP address__ with a device connected to the same network and as path the command you want to execute. 
For example: 
  - _192.168.0.0/play_ starts the play command
  - _192.168.0.0/next_ plays the next song
  - _192.168.0.0/prev_ plays the previous song
  
## IR-sensor
By connecting an IR-sensor to the ESP-module, it is possible to control the playback. Because I couldn't detect the protocol used by the IR-remote that I used, I've implemented a custom protocol using three bits, three opportunities for the user to send a signal once the detecting phase has been started. 
