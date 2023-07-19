#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "credentials.h"


String clientID = "ESP32-Weather";
const char *mqtt_server = "iot21-node-red.duckdns.org";
const char *mqtt_user = "My-Mosqt-server";
const char *mqtt_password = "MyCentMosqP@ss";
WiFiClient espclient; 
PubSubClient client(espclient);

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attampting MQTT connection............");
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), mqtt_user, mqtt_password))
        {
            Serial.print("connect to MQTT");
           // client.subscribe("fan"); //broker subscription 1
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print(" try again in 5 sec");
            delay(5000);
        }
    }
}

void callback(char *topic, byte *message, unsigned int length)
{
    Serial.println("Message arrived on topic : ");
    Serial.println(topic);
    Serial.println(". Message: ");
    String messageTemp;

    for(uint16_t i=0; i<length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
/*
    if (String(topic) == "fan"){ //subscription od fan topic
        if(messageTemp == "true"){
            Serial.println("The fan is start");
        }
        else if(messageTemp == "false"){
            Serial.println("The fan is stop");
        }
    }
    */
 
}

void connectAP()
{
    Serial.println("Connect to my WiFi");
    WiFi.begin(ssid, password);
    byte cnt=0;

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        cnt++;

        if(cnt>30)
        {
            ESP.restart();
        }
    }
}