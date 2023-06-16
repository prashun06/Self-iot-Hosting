#include <Arduino.h>
#include <ArduinoJson.h>
#include <wifimqtt.h>
#include "main.h"


unsigned long previousMillis=millis();

void sendMQTTvalues(){ // send the json data from esp to nodered 
  StaticJsonDocument<96> doc;

  doc["device"] = "ESP MCU";
  doc["temperature"] = 33;
  doc["humidity"] = 90;
  doc["lux"] = 643;
  
  char buff[256];
  serializeJson(doc, buff); 
  client.publish("ESP_values",buff);
}

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
    //client.publish("toNodeRED","Hellow from esp32"); // it will send the data to mosquitto broker 
    sendMQTTvalues();
  }
}
