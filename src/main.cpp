#include <Arduino.h>
#include <ArduinoJson.h>
#include <wifimqtt.h>
#include "main.h"


unsigned long previousMillis=millis();

void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() 
{
  if(!client.connected()){
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP32-1");
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= 5000){
    previousMillis = currentMillis;
    //digitalWrite(led,!digitalRead(led));
    client.publish("toNodeRED","Hellow from esp32"); // it will send the data to mosquitto broker 
  }
}
