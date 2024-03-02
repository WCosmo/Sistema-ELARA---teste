/*
 * Project: Sistema ELARA - Sistema Embarcado LoRa para Análise Remota Ambiental (test mode)
 * Device: RAK LoRa Endnode 
 * Author: Wilson Cosmo
 */

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "nome"; //name of the WiFi network
const char* password = "senha"; //WiFi password

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

//Endnodes list:
String tx_id_1 = "T1";
String tx_id_2 = "T2";
String tx_id_3 = "T3";
String tx_id_4 = "T4";
String tx_id_5 = "T5";
String tx_id_6 = "T6";
String tx_id_7 = "T7";
String tx_id_8 = "T8";
String tx_id_9 = "T9";

//Your Domain name with URL path or IP address with path
String serverName = "https://api.thingspeak.com/update?api_key=";
String thingspeak_key = "6NHWH4LJEZT4JXAB";  /* Coloque aqui sua chave de escrita do seu canal */

void set_api_key(String idd){
  if(idd == tx_id_1){
    thingspeak_key = "6NHWH4LJEZT4JXAB";
  }
  if(idd == tx_id_2){
    thingspeak_key = "";
  }
  if(idd == tx_id_3){
    thingspeak_key = "";
  }
  if(idd == tx_id_4){
    thingspeak_key = "";
  }
  if(idd == tx_id_5){
    thingspeak_key = "";
  }
  if(idd == tx_id_6){
    thingspeak_key = "";
  }
  if(idd == tx_id_7){
    thingspeak_key = "";
  }
  if(idd == tx_id_8){
    thingspeak_key = "";
  }
  if(idd == tx_id_9){
    thingspeak_key = "";
  }

}

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

void wifi_start(){

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void setup() {
  
  Serial.begin(115200);
  while (!Serial);

  Serial.println("=====================================");    
  Serial.print("Sistema ELARA\nDispositivo: Gateway");   
  Serial.println("=====================================");

  LoRa.setPins(SS,RST,DI0);

  if (!LoRa.begin(915000000)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(12);
  Serial.println("LoRa receiver ok"); 

  wifi_start();

  Serial.println("\nLoRa Gateway ready.\n\n");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    String incoming = "";    

    // read packet
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    
    if(incoming.startsWith(tx_id_1) || incoming.startsWith(tx_id_2) || incoming.startsWith(tx_id_3) || incoming.startsWith(tx_id_4) || incoming.startsWith(tx_id_5) || incoming.startsWith(tx_id_6) || incoming.startsWith(tx_id_7) || incoming.startsWith(tx_id_8) || incoming.startsWith(tx_id_9)){
      //Serial.print("Received packet: '");
      Serial.print(incoming);
      Serial.print(";");
      Serial.print(String(LoRa.packetRssi()));
      Serial.print(";");
      Serial.println(String(LoRa.packetSnr()));
      

      String s_payload = incoming;

      //Tx ID:
      int f1 = s_payload.indexOf(';');      
      set_api_key(s_payload.substring(0,f1));

      //Tag:
      int f2 = s_payload.indexOf(';', f1 + 1 );             
      
      //Battery Voltage
      int f3 = s_payload.indexOf(';', f2 + 1 );      

      //Temperature
      int f4 = s_payload.indexOf(';', f3 + 1 );      

      //ph
      int f5 = s_payload.indexOf(';', f4 + 1 );      

      //var 3
      int f6 = s_payload.indexOf(';', f5 + 1 );      

      //var 4
      int f7 = s_payload.indexOf(';', f6 + 1 );      

      //Send via HTTP post
      if ((millis() - lastTime) > timerDelay) {
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;

        String serverPath = serverName + thingspeak_key + "&field1=" + s_payload.substring(f1+1,f2) + "&field2=" + s_payload.substring(f2+1,f3) + "&field3=" + s_payload.substring(f3+1,f4) + "&field4=" + s_payload.substring(f4+1,f5) + "&field5=" + s_payload.substring(f5+1,f6) + "&field6=" + s_payload.substring(f6+1,f7) + "&field7=" + String(LoRa.packetRssi()) + "&field8=" + String(LoRa.packetSnr());
        Serial.print("HTTP post: ");
        Serial.println(serverPath);
        
        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        
        // If you need Node-RED/server authentication, insert user and password below
        //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
        wifi_start();
      }
      lastTime = millis();
    }
      
    }
    else{
      Serial.println("Invalid ID!");
    }  
  }
}
