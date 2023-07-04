#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15 
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================


#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

//
#define FLASH_LED 4

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "toomuch2";
const char* password = "pfil1234";

const char *HostName = "nodeLED Nodered";
const char *mqtt_server = "iot21-node-red.duckdns.org";
const char *mqtt_user = "My-Mosqt-server";
const char *mqtt_password = "MyCentMosqP@ss";
bool useMQTT =true;
const char* topic_photo = "takePhoto";
const char* topic_publish = "sendPhoto";
const char* topic_Flash = "OnFlash";
const int MAX_PAYLOAD = 60000;

bool flash;

WiFiClient espclient;
PubSubClient client(espclient);

void startCameraServer();
void setupLedFlash(int pin);
void callback(String topic, byte* message, unsigned int length);
void sendMQTT(const uint8_t *buf, uint32_t len);
void take_photo();
void on_flash();
void reconnect();

void setup() {

  // Define Flash as an output
  pinMode(FLASH_LED, OUTPUT);

  // Initialise the Serial Communication
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Config Camera Settings
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  flash = true;

  // Not used in our project
  #if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  #endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  // Not used in our project
  #if defined(CAMERA_MODEL_M5STACK_WIDE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  #endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Set MQTT Connection
  client.setServer(mqtt_server, 1883);
  client.setBufferSize (MAX_PAYLOAD); //This is the maximum payload length
  client.setCallback(callback);
}


void callback(String topic, byte* message, unsigned int length){
    
    //Serial.println(topic);
    String messageTemp;
    Serial.println(topic);
    for(int i=0; i<length; i++)
    {
      //Serial.print((char)message[i]);
      messageTemp += (char)message[i];
    }

    if (topic == topic_photo){ //subscription od fan topic
      Serial.println(" photo taking ");
      take_photo();
    }
    else if(topic == topic_Flash){
      Serial.println(" flashing ");
      on_flash();
    }
  }

void sendMQTT(const uint8_t * buf, uint32_t len){
  Serial.println("Sending picture...");
  if(len>MAX_PAYLOAD){
    Serial.print("Picture too large, increase the MAX_PAYLOAD value");
  }else{
    Serial.print("Picture sent ? : ");
    Serial.println(client.publish(topic_publish, buf, len, false));
  }
}

void take_photo() {
  camera_fb_t * fb = NULL;
  if(flash){ digitalWrite(FLASH_LED, HIGH);};
  Serial.println("Taking picture");
  fb = esp_camera_fb_get(); // used to get a single picture.
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  Serial.println("Picture taken");
  digitalWrite(FLASH_LED, LOW);
  sendMQTT(fb->buf, fb->len);
  esp_camera_fb_return(fb); // must be used to free the memory allocated by esp_camera_fb_get().
  
}

void on_flash(){
  flash = !flash;
  Serial.print("Setting flash to ");
  Serial.println(flash);
  if(!flash){
    for(int i=0; i<6; i++){
      digitalWrite(FLASH_LED, HIGH);
      delay(100);
      digitalWrite(FLASH_LED, LOW);
      delay(100);
    }
  }
  if(flash){
    for(int i=0; i<3; i++){
      digitalWrite(FLASH_LED, HIGH);
      delay(500);
      digitalWrite(FLASH_LED, LOW);
      delay(100);
    }
}
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HostName, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_photo);
      client.subscribe(topic_Flash);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}