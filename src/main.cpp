//https://github.com/schreibfaul1/ESP32-audioI2S/tree/master


#include <Arduino.h>
#include <ArduinoJson.h>
#include "Audio.h"
#include <wifimqtt.h>

//define I2S
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

Audio audio;

void setup(){
  Serial.begin(115200);
  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(10); //0-100
  audio.connecttohost("https://onair.krone.at/kronehit.mp3"); //radio connection
}

void loop() 
{
  if(!client.connected()){
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP32-1");
  }

  audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
    Serial.println(info);
    client.publish("iradio/streamtitle",info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
