/*********
  https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  sudo chmod a+rw /dev/ttyUSB0  
*********/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ultrasonic.h>

Ultrasonic ultrasonic(5, 4); //pins 1 & 2 
int distance;
int statusCode = 2;

const int relay = 16; //0
const int buzzer = 0; //3
const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(relay, OUTPUT);
  pinMode(buzzer,OUTPUT);

  digitalWrite(relay, HIGH);
  digitalWrite(buzzer,LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  buzzTone(70);

  server.on("/door", handleDoor);
 
  server.on("/", handleRootPath);    //Associate the handler function to the path
  server.begin();                    //Start the server
  Serial.println("Server listening");
 
}
void loop(){
  server.handleClient();   
}
 
void handleRootPath() {  
  doorState();
  Serial.println(statusCode);

  if (statusCode == 0){
    server.send(200, "text/html","<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><link rel=\"icon\" href=\"data:,\"/><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}.button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}.button2 {background-color: #77878A;}</style></head><body><h1>Garage Door Opener</h1><h2>Door state: <span style=\"background-color:#78b159\">Open</span></h2><p/><form action=\"/door\"><input class=\"button\" type=\"submit\" value=\"Press\"/></form><p/></body></html>");
  } else if (statusCode == 1){
    server.send(200, "text/html","<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><link rel=\"icon\" href=\"data:,\"/><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}.button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}.button2 {background-color: #77878A;}</style></head><body><h1>Garage Door Opener</h1><h2>Door state: <span style=\"background-color:#dd2e44\">Closed</span></h2><p/><form action=\"/door\"><input class=\"button\" type=\"submit\" value=\"Press\"/></form><p/></body></html>");
  } else {
    server.send(200, "text/html","<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><link rel=\"icon\" href=\"data:,\"/><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}.button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}.button2 {background-color: #77878A;}</style></head><body><h1>Garage Door Opener</h1><h2>Door state: <span style=\"background-color:#fdcb58\">Unknown</span></h2><p/><form action=\"/door\"><input class=\"button\" type=\"submit\" value=\"Press\"/></form><p/></body></html>");
  }      
}

void handleDoor(){
  doorHandler();
  server.sendHeader("Location", String("/"), true);
  server.send( 302, "text/plain", "");
}

unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 4000;

void doorHandler(){
  
  if (millis() - previousTime <= timeoutTime) {
    Serial.println("Ratelimit hit");
    return;
  }

  buzzTone(400);
  digitalWrite(relay, LOW);
  delay(500);
  digitalWrite(relay, HIGH);
  
  Serial.print("Ratelimit passed");
  previousTime = millis();

}

/**
3-4 = close
255 = lowest open
265 = highest open

0 = door open
1 = door closed
2 = error 
**/

const int close = 4;
const int lowestOpen = 255;
const int highestOpen = 265;

void doorState(){

  distance = ultrasonic.read();
  Serial.println(distance);
  if (distance <= close){
    statusCode = 1;
  } else if (distance >= lowestOpen || distance <= highestOpen){
    statusCode = 0;
  } else {
    statusCode = 2;
  }
}


void buzzTone(int delayTime){
  digitalWrite(buzzer,HIGH);
  delay(delayTime);
  digitalWrite(buzzer,LOW);

}




