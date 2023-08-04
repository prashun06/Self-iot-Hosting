#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "credentials.h"
#include "globals.h"

String clientID = "ESP32-1";
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
            Serial.println("connect to MQTT");
            client.subscribe("pumpstart"); //broker subscription 1

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
    String messageTemp;
    //Serial.println(topic);

    for(int i=0; i<length; i++)
    {
        messageTemp += (char)message[i];
    }

    if (String(topic) == "pumpstart"){ //subscription 1
        if( messageTemp == "1"){
            Serial.println("pumping.....");
            previousMillis = millis();
            state=PUMPING;
        }
        else if( messageTemp == "0"){
            Serial.println("sleeping.....");
            state=TIMEFORSLEEP; 
        }
    }
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