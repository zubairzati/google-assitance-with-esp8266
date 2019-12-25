#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define RELAY2          D2
#define RELAY1          D1

#define WLAN_SSID      "xxxx" //your wife name
#define WLAN_PASS      "xxxxxx" // your wifi password

#define AIO_SERVER  "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME  "xxxxxx" // type here your username of adafruit.io
#define AIO_KEY     "xxxxxxxxxxxxxxxxxxxxxx" // type here api key 

WiFiClient client;

Adafruit_MQTT_Client  mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY );
// setup the MQTT client class by passing in the wifi client and mqtt server and login details.

//setup a feed called 'on off' for subscribing to changes.
Adafruit_MQTT_Subscribe Light1 =Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/RELAY1");
Adafruit_MQTT_Subscribe Light2 =Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/RELAY2");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  // CONNECT TO WIFI ACCESS POINT
  Serial.println();  Serial.println();
  Serial.print("Connecting to");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID,WLAN_PASS);
  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription for on off feed.
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);
}

void loop() 
{
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Light1) {
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(RELAY1, Light1_State);
    }
    if (subscription == &Light2) {
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(RELAY2, Light2_State);
    }}}
    
void MQTT_connect() //MQTT BROKER DISCONNECTS THE CLIENT WHICH IS NOT TRANSMITTING ANY DATA 
{
  int8_t ret; 
  if (mqtt.connected())  // Stop if already connected.
  {    return;  }

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) 
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) 
    {
      // basically die and wait for WDT(watch dog timer)to reset me
      while (1);
    }}
  Serial.println("MQTT Connected!");
}
