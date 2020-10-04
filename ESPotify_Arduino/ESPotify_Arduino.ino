/*
 * A file named 'secrets.h' must be created in this directory containing: 
 *    * #define MY_SSID "<wifi ssid>"
 *    * #define MY_PASS "<wifi password>"
 *    * #define MY_CLIENT_ID_SECRET_64 "< "<CLIENT_ID:CLIENT_SECRET>" encoded in base64>"
 *    * #define MY_REFRESH_TOKEN "<Personal Spotify refresh token>"
 *    * #define MY_DEVICE_ID "<The Spotify ID of the target device>"
 */

/* ----------------------------
  Imported libraries
 ---------------------------- */
#include <ESP8266WiFi.h>                                          // Responsible for connecting to a WiFi network.
#include <WiFiClientSecure.h>                                     // Responsible for communication with API via HTTP requests.
#include <ESP8266WebServer.h>                                     // Responsible for handling the web server.
#include <ArduinoJson.h>                                          // Responsible for the interpretation of json objects.
#include <NTPClient.h>                                            // Responsible for the handling of time.
#include <WiFiUdp.h>                                              // ^
#include "secrets.h"                                              // File with the credentials.
#include <PubSubClient.h>                                         // MQTT helper

/* ----------------------------
  Web server
 ---------------------------- */
// ESP8266WebServer server(80);                                      // Web server at port 80 (default).
    
/* ----------------------------
  Time implementation
 ---------------------------- */
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
unsigned long currentTime;                                        // Time to test for an elapsed time.
unsigned long lastRefreshTime;                                    // Reference time of last refresh of the access token.
unsigned long differenceRefresh;                                  // Difference in time between two refresh.

/* ----------------------------
  Wifi connections
 ---------------------------- */
char ssid[] = MY_SSID;                                            // SSID
char password[] = MY_PASS;                                        // Password
//WiFiClient espClient;
WiFiClient client;                                                // Client handling HTTP requests.


/* ----------------------------
  Spotify API variables
 ---------------------------- */
String token = "";                                                // Access token for Spotify authorization.
const String VIBES = "0puuabIopXq11W5M2JwKbc";                    // Playlist ID
const String DISCO = "7Ko9ru5QQ2uWZCJgsWdQYo";                    // Playlist ID
const String TECHNO = "45yj0IEVvyZd4ozGzlVnFO";                   // Playlist ID
const String ROCK = "2BaS1D1GKRyEmoTgs5lVPw";                     // Playlist ID
const String RAP = "2GiLXbCqu3Q5szGUpcnwZC";                      // Playlist ID
const String LATIN = "32XtH3Po7ahVOyUPD2Jjon";                    // Playlist ID
const String THIS = "04GLMFMWhHl1vqxkDwjwwZ";                     // Playlist ID
const String DNB = "0HIqr9tu70WeDnHndkDPK8";                      // Playlist ID
const String THOMAS = "4o1ArjWP3qX3xOQZX4K1wv";                   // Playlist ID
const String TWOTEN = "3oZlbdnVk981T9PuobSuhm";                   // Playlist ID
const String TWOFOUR = "1K6RH6pFSAFGaRNnYcjdlY";                  // Playlist ID
#define HOST "api.spotify.com"                                    // Host for the HTTP requests.
#define CLIENT_ID_SECRET_64 MY_CLIENT_ID_SECRET_64
#define REFRESH_TOKEN MY_REFRESH_TOKEN  
#define DEVICE_ID MY_DEVICE_ID                     

/* ----------------------------
  Handling HTTP response
 ---------------------------- */
static char respBuffer[4096];                                     // Buffer containing the response of the HTTP request.

/* ----------------------------
  Creating a HTML page
 ---------------------------- */
const String htmlPageBegin = "<!DOCTYPE html> <html> <head> <title>ESP8266</title> </head> <body> <center> <h1>";
const String htmlPageEnd   = "</h1> <hr> <p>Requesting command... </center> </body> </html>"; 

/* ----------------------------
  IR sensor
 ---------------------------- */
const int pin = 14;
int currentSignal;
unsigned long diff = 0;
unsigned long initial;
int code[3];
boolean playing = false;


// ------------------------------------------------------------------------------------------------------------ void SETUP ----------------------------------------------------
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin, INPUT);

  // LED is on during setup.
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("*");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

//  client.setInsecure();

//  server.on("/pause", pause);
//  server.on("/play", play);
//  server.on("/next", next);
//  server.on("/prev", prev);
  
//  server.on("/token", postRequestToken);
  
  // Playlists
//  server.on("/vibes", playVibes);
//  server.on("/disco", playDisco);
//  server.on("/techno", playTechno);
//  server.on("/rock", playRock);
//  server.on("/rap", playRap);
//  server.on("/latin", playLatin);
//  server.on("/this", playThis);
//  server.on("/dnb", playDnb);
//  server.on("/thomas", playThomas);
//  server.on("/2010", playTwoten);
  
  // server.begin();
  Serial.println("Server started!");

  timeClient.begin();
  timeClient.setTimeOffset(0);

  timeClient.update();
  lastRefreshTime = timeClient.getEpochTime();

  postRequestToken();

  // Setup has finished and LED turns of
  digitalWrite(LED_BUILTIN, HIGH);

}


// ------------------------------------------------------------------------------------------------------------ String HTML ------------------------------------------------------
String createHtml(String msg){
  return String(htmlPageBegin) + String(msg) + String(htmlPageEnd);
}

// ------------------------------------------------------------------------------------------------------------ void blinkLED ----------------------------------------------------
void blinkLED(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
}

// ------------------------------------------------------------------------------------------------------------ void PUT REQUEST ----------------------------------------------------
void putRequest(String cmd) {
  blinkLED();
  Serial.println("Made request"); Serial.println(cmd);
  
  // server.send(200, "text/html", createHtml(cmd));

  if (!client.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("PUT "));
  client.print("/v1/me/player/");
  client.print(cmd);
  client.print("?device_id=");
  client.print(DEVICE_ID);
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(HOST);
  client.println(F("Accept: application/json"));
  client.println(F("Content-Type: application/json"));
  client.print(F("Authorization: Bearer "));
  client.println(token);
  client.print(F("Content-Length: "));
  client.println(0);
  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
}


// ------------------------------------------------------------------------------------------------------------ void POST REQUEST ----------------------------------------------------
void postRequest(String cmd){
  blinkLED();
  Serial.print("Made request: "); Serial.println(cmd);

  // server.send(200, "text/plain", cmd);

  if (!client.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("POST "));
  client.print("/v1/me/player/");
  client.print(cmd);
  client.print("?device_id=");
  client.print(DEVICE_ID);
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(HOST);
  client.println(F("Accept: application/json"));
  client.println(F("Content-Type: application/json"));
  client.print(F("Authorization: Bearer "));
  client.println(token);
  client.print(F("Content-Length: "));
  client.println(0);
  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
}


// ------------------------------------------------------------------------------------------------------------ void POST REQUEST TOKEN -------------------------------------------
void postRequestToken(){
  Serial.println("Refreshing token");
  digitalWrite(LED_BUILTIN, LOW);
  // server.send(200, "text/plain", "REFRESH");

  if (!client.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  String data = String("grant_type=refresh_token&refresh_token=") + REFRESH_TOKEN;
  int dataLength = data.length();

  // Send HTTP request
  client.print(F("POST "));
  client.print("/api/token");
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println("accounts.spotify.com");
  client.println(F("Accept: application/json"));
  client.println(F("Content-Type: application/x-www-form-urlencoded"));
  client.print(F("Authorization: Basic "));
  client.println(CLIENT_ID_SECRET_64);
  client.print(F("Content-Length: "));
  client.println(dataLength);
  client.println();
  client.println(data);

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }

  uint16_t index = 0;
  // While the client is still availble read each
  // byte and print to the serial monitor
  while (client.available()) {

    respBuffer[index++] = client.read();
    delay(1);

  }
  client.stop();
  char * json = strchr(respBuffer, '{');

  DynamicJsonBuffer jBuffer;
  JsonObject& root = jBuffer.parseObject(json);

  String newToken = root["access_token"];
  token = newToken;
 
  Serial.println("New token assigned successfully.");

  digitalWrite(LED_BUILTIN, HIGH);
  
}

// ------------------------------------------------------------------------------------------------------------ void PLAY FROM PLAYLIST -------------------------------------------
void playFromPlaylist(String playlistID){
  blinkLED();
  Serial.print("Made request: "); Serial.println(playlistID);

  // server.send(200, "text/plain", playlistID);

  if (!client.connect(HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  String playlistURI = String("spotify:playlist:") + playlistID;
  String data = "{\"context_uri\":\"" + playlistURI + "\"}";        // Json format
  int dataLength = data.length();

  // Send HTTP request
  client.print(F("PUT "));
  client.print("/v1/me/player/play?device_id=");
  client.print(DEVICE_ID);
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(HOST);
  client.println(F("Accept: application/json"));
  client.print(F("Authorization: Bearer "));
  client.println(token);
  client.println(F("Content-Type: application/json"));
  client.print(F("Content-Length: "));
  client.println(dataLength);
  client.println();
  client.println(data);

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
}


// ------------------------------------------------------------------------------------------------------------ void PREV ----------------------------------------------------
void prev(){
  postRequest("previous");
}

// ------------------------------------------------------------------------------------------------------------ void NEXT ----------------------------------------------------
void next(){
  postRequest("next");
}

// ------------------------------------------------------------------------------------------------------------ void PLAY ----------------------------------------------------
void play(){
  putRequest("play");
}

// ------------------------------------------------------------------------------------------------------------ void PAUSE ----------------------------------------------------
void pause(){
  putRequest("pause");
}

// ------------------------------------------------------------------------------------------------------------ void PLAY PLAYLISTS ----------------------------------------------
void playVibes(){
  blinkLED();
  playFromPlaylist(VIBES);
}
void playDisco(){
  blinkLED();
  playFromPlaylist(DISCO);
}
void playTechno(){
  blinkLED();
  playFromPlaylist(TECHNO);
}
void playRock(){
  blinkLED();
  playFromPlaylist(ROCK);
}
void playRap(){
  blinkLED();
  playFromPlaylist(RAP);
}
void playLatin(){
  blinkLED();
  playFromPlaylist(LATIN);
}
void playThis(){
  blinkLED();
  playFromPlaylist(THIS);
}
void playDnb(){
  blinkLED();
  playFromPlaylist(DNB);
}
void playThomas(){
  blinkLED();
  playFromPlaylist(THOMAS);
}
void playTwoten(){
  blinkLED();
  playFromPlaylist(TWOTEN);
}
void playTwoFour(){
  blinkLED();
  playFromPlaylist(TWOFOUR);
}


// ------------------------------------------------------------------------------------------------------------ void checkSignal ----------------------------------------------------
void checkSignal(){
  currentSignal = digitalRead(pin);

  if (currentSignal == 0){

    code[0] = 0;
    code[1] = 0;
    code[2] = 0;
    
    diff = 0;
    
    
    delay(750);
    digitalWrite(LED_BUILTIN, LOW);delay(50);digitalWrite(LED_BUILTIN, HIGH);
    timeClient.update();
    initial = timeClient.getEpochTime();
    
    while(diff < 1.5){
      currentSignal = digitalRead(pin);
      if (currentSignal == 0){
        code[0] = 1;
      }
      timeClient.update();
      diff = abs(timeClient.getEpochTime() - initial);
//      Serial.println(diff); 
    }

    digitalWrite(LED_BUILTIN, LOW);delay(50);digitalWrite(LED_BUILTIN, HIGH);
    
    while (diff < 2.5){
      currentSignal = digitalRead(pin);
      if (currentSignal == 0){
        code[1] = 1;
      }
      timeClient.update();
      diff = abs(timeClient.getEpochTime() - initial);
    }

    digitalWrite(LED_BUILTIN, LOW);delay(50);digitalWrite(LED_BUILTIN, HIGH);
    
    while (diff < 3.25){
      currentSignal = digitalRead(pin);
      if (currentSignal == 0){
        code[2] = 1;
      }
      timeClient.update();
      diff = abs(timeClient.getEpochTime() - initial);
    }

    Serial.print("Recieved code: "); Serial.print(code[0]); Serial.print(code[1]); Serial.println(code[2]);

    if (code[0] == 0 && code[1] == 0 && code[2] == 0){              // 000
      if (playing){
        pause();
      } else {
        play();
      }
      playing = !playing;
      return;
    }
         
    else if (code[0] == 1 && code[1] == 0 && code[2] == 0){         // 100
      next();
      playing = true;
      return;
    }

    else if (code[0] == 0 && code[1] == 1 && code[2] == 0){         // 010
      prev();
      playing = true;
      return;
    }

    else if (code[0] == 0 && code[1] == 0 && code[2] == 1){         // 001
      playFromPlaylist(TWOFOUR);
      playing = true;
      return;
    }

    else if (code[0] == 1 && code[1] == 1 && code[2] == 0){         // 110
      playFromPlaylist(RAP);
      playing = true;
      return;
    }

    else if (code[0] == 1 && code[1] == 0 && code[2] == 1){         // 101
      playFromPlaylist(THIS);
      playing = true;
      return;
    }

    else if (code[0] == 1 && code[1] == 1 && code[2] == 1){         // 111
      playFromPlaylist(VIBES);
      playing = true;
      return;
    }

    else if (code[0] == 0 && code[1] == 1 && code[2] == 1){         // 011  
      playFromPlaylist(LATIN);
      playing = true;
      return;
    }
  }
}

// ------------------------------------------------------------------------------------------------------------ void LOOP ----------------------------------------------------
void loop() {

  checkSignal();
  timeClient.update();
  currentTime = timeClient.getEpochTime();
  differenceRefresh = abs(currentTime - lastRefreshTime);
  
  if (differenceRefresh > 3500){
    postRequestToken();
    lastRefreshTime = currentTime;
  } 

  //server.handleClient();
  
}
