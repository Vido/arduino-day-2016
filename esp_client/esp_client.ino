#include <DHT.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>
#include "simple_http.h"

// Important parameters
#define IPADDR "192.168.0.131"
#define TCPPORT 8266
#define SSID "dlink"
#define PASSWD "123donawilma456"

#define ESP_SERIAL Serial
ESP8266 wifi(ESP_SERIAL); // ESP object
DHT dht;                  // Sensor object

void setup()
{
  dht.setup(2); // data pin 2
  ESP_SERIAL.begin(115200);
  
  // Set STATION
  wifi.setOprToStation();
  wifi.disableMUX();
}

void loop()
{  
  // Reads the sensor
  uint8_t humidity = dht.getHumidity();
  uint8_t temperature = dht.getTemperature();

  // Builds the HTTP package
  char json_payload[64]="";
  char tcp_payload[HTTP_PAYLOAD_SIZE]="";
  dht_json_wrapper(json_payload, 64, dht.getHumidity(), dht.getTemperature());
  http_wrapper(tcp_payload, HTTP_PAYLOAD_SIZE, json_payload, strlen(json_payload));

  // Joins the AP
  bool joinap_error = wifi.joinAP(SSID, PASSWD);
  if(joinap_error==false){
    wifi.leaveAP();
    return;
  }

  // Sends data
  bool tcp_error = wifi.createTCP(IPADDR, TCPPORT);
  // Create TCP connection in single mode.
  // Casting is needed
  wifi.send((uint8_t *)tcp_payload, (uint32_t) strlen(tcp_payload));
  wifi.releaseTCP();

  wifi.leaveAP();

  delay(1000);
}
