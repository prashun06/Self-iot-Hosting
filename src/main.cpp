#include <Arduino.h>
#include <ArduinoJson.h>
#include <wifimqtt.h>

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D8
#define RST_PIN D4

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

int statuss = 0;
int out = 0;
String tag;

bool ckeckIn = true;

unsigned long previousMillis=millis();


void setup(){
	Serial.begin(115200);
  SPI.begin(); 
  mfrc522.PCD_Init();
  Serial.println("RFID is ready");

  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {  // Look for new cards

  if(!client.connected()){
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP32-1");
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  //byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == "8E 39 32 50") //change UID of the card that you want to give access
  {
    Serial.println(" ESP32 ");
    delay(1000);
    statuss = 1;
    client.publish("time/esp", "#esp32");
  }
  
  else   {
    Serial.println(" Access Denied ");
    delay(1000);
    client.publish("time/blank", "#Access Denied");
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= 5000){
    previousMillis = currentMillis;
    //digitalWrite(led,!digitalRead(led));
    //client.publish("toNodeRED","Hellow from esp32"); // it will send the data to mosquitto broker 
  }
}
