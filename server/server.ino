
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "config.h"
#include "webserver.h"
#include <PubSubClient.h>

const char* ssid = "zarza wifi";
const char* password = "alberto123456";

/* const char* mqttServer = "m15.cloudmqtt.com";
  const int mqttPort = 10396;
  const char* mqttUser = "altegzoe";
  const char* mqttPass = "X5V9Dh-ci8Ts";*/

config_t conf;

WiFiClient espClient;
PubSubClient clientMQTT(espClient);
int timer = 0;
bool lastState = false;



//DNSServer dnsServer;


//IPAddress apIP(192, 168, 0, 200);
//IPAddress netMsk(255, 255, 255, 0);

int led = 13;

void setup(void) {

  Serial.begin(115200);
  pinMode(13, INPUT_PULLUP);

  //strcpy(conf.essid, ssid);
  //strcpy(conf.pass, password);

  EEPROM.begin(sizeof(config_t));
  //EEPROM.put(0, conf);
  //EEPROM.commit();

  EEPROM.get(0, conf);
  Serial.println("Datos de red:");
  Serial.println(conf.essid);
  Serial.println(conf.pass);
  Serial.println("MQTT:");
  Serial.println(conf.broker_add);



  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());





  /*while (!clientMQTT.connected()) {
    delay(500);
    Serial.print(".");
    }*/

  //Serial.println("MQTT Connected!");





  //WiFi.softAPConfig(apIP, apIP, netMsk);
  //WiFi.softAP("TestIOT-rmm", "");

  // Wait for connection

  //Serial.print("IP address: ");
  //Serial.println(WiFi.softAPIP());

  /*if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
    } else {
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    }*/
  WebServer_init();

  //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  //dnsServer.start(53, "*", apIP);

  Serial.println("HTTP server started");
  Serial.println("COnnecting MQTT");
}

void loop(void) {
  //DNS
  //dnsServer.processNextRequest();
  //HTTP
  WebServer_loop();

  timer++;
  delay(10);

  bool currState = digitalRead(13);

  if (!clientMQTT.connected()) {
    int timeout = 10;
    EEPROM.get(0, conf);

    clientMQTT.setServer(conf.broker_add, conf.broker_puerto);
    clientMQTT.connect(conf.clientID, conf.broker_user, conf.broker_pass);

    Serial.println("Connection MQTT");

    while (!clientMQTT.connected() && timeout--) {
      Serial.print(".");
      delay(500);
    }
  }

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);
  char result[8]; // Buffer big enough for 7-character float
  dtostrf(voltage, 6, 2, result); // Leave room for too large numbers!

  clientMQTT.publish(conf.broker_topic, result);

}
