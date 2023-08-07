#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "credentials.h"

String clientID = "ESP32-1";
const char *mqtt_server = "iot21-node-red.duckdns.org";
const char *mqtt_user = "My-Mosqt-server";
const char *mqtt_password = "MyCentMosqP@ss";
WiFiClient espclient;
PubSubClient client(espclient);

extern Audio audio;

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attampting MQTT connection............");
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), mqtt_user, mqtt_password))
        {
            Serial.println("connect to MQTT");
            client.subscribe("iradio/pauseResume"); //broker subscription 1
            client.subscribe("iradio/setValume");
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
        Serial.println((char)message[i]);
        messageTemp += (char)message[i];
    }

    if (String(topic) == "iradio/pauseResume"){ //subscription 1
        audio.pauseResume();
        //Serial.println(audio.showstreamtitle());
    }
    else if (String(topic) == "iradio/setValume"){
        audio.setVolume(messageTemp.toInt());
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
