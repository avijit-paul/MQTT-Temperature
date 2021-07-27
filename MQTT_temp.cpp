// partialy taken from Arduino Library example
// defining header files
#include "WiFi.h"
#include <AsyncTCP.h> 
#include <AsyncMqttClient.h>
#include <Adafruit_BME280.h>

// #define HOME 

#ifdef HOME
#define WIFI_SSID "xxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxx"
#else
#define WIFI_SSID "xxxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxxx"
#endif 

// defining IP addresses

#ifdef HOME
#define MQTT_HOST IPAddress(192, 168, *, *) 
#else
#define MQTT_HOST IPAddress(192, 168, *, *) 
#endif

#define MQTT_PORT xxxx // use own port

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
 
AsyncMqttClient mqttClient;
Adafruit_BME280 bme; // define sensor data


void setup() {
  // start serial output
  Serial.begin(115200);

  // set up wifi
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(ADNGROUP);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED)
  {
      Serial.println(WiFi.status());
      delay(500);
  }
  Serial.println(WiFi.localIP());

  // set up mqtt client
  mqttClient.onMessage(onMqttMessage); 
  mqttClient.setClientId(ADNGROUP);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT); 
  mqttClient.connect();

  
  // init temp sensor
  bme.begin(0x76);
}

void loop() {
  char temp_b[4];
  float temp_f;
  
  // check if mqtt is connected
  while(!mqttClient.connected ())
  {
    Serial.print("."); 
    delay(500); // in miliseconds
  }

  // measure temperature 
  temp_f = bme.readTemperature();

  Serial.println( sizeof(temp_f) );
  Serial.println( temp_f );

  memcpy(temp_b, &temp_f, sizeof(temp_b));
   
  if(mqttClient.connected()){
  // publish( topic, QoS, retain flag, payload*, length, dup flag )    
    mqttClient.publish( "mqtt/temp", 1, true, temp_b, sizeof(temp_b), 0);
  }
  Serial.println( temp_b );

  // every 5 sec
  delay( 5000 );
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}
