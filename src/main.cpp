#include <Arduino.h>
#include <ArduinoJson.h>
#include <wifimqtt.h>
#include "ultransonic.h"



void getData(){
  client.publish("pumpinfo", "give me info");
  Serial.println("mqtt publish info");
  state = WAIT;
}
void sendNewPumpinfo(){
  StaticJsonDocument<48> doc;

  doc["distance1"] = distance1;
  //doc["distance1"] = distance1;

  char buffer_1[256];
  serializeJson(doc, buffer_1);

  client.publish("Water_Distance", buffer_1);
  Serial.println("Distance transmitted");
}


void pumping(){
   unsigned long currentMillispump = millis();
   //Serial.println("pumpstart");

  if(currentMillispump - previousMillis >= 5000){
     Serial.println("pumpstoped");
     client.publish("pumpinfo", "pumpstoped");
     getDistance();
     Serial.println(distance1);
     sendNewPumpinfo();
     state = TIMEFORSLEEP;
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(trigger1,OUTPUT);
  pinMode(echo1,INPUT);
  state = GETDATA;
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

  switch (state)
  {
  case GETDATA:
    getData();
    break;
  
  case PUMPING:
  pumping();
  break;

  case WAIT:
  //Serial.println("waiting");
  break;

  case TIMEFORSLEEP:
  Serial.println("going to sleep");
  client.publish("deepsleep", "going to sleep");
  delay(1000);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  delay(10000);
  state = GETDATA;
  break;
  
  default:
    break;
  }

  // unsigned long currentMillis = millis();
  // if(currentMillis - previousMillis >= 5000){
  //   previousMillis = currentMillis;
  //   //digitalWrite(led,!digitalRead(led));
  //   //client.publish("toNodeRED","Hellow from esp32"); // it will send the data to mosquitto broker 
  //   sendMQTTvalues();
  // }
}
