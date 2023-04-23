#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include "pictureText.h"
#define LED 5 //<<-- config GPIO

//@ Config wifi here!!
const char *ssid = "SettingSSID";  //<--  
const char *password = "SettingPassword";  //<-- 
//@ Config wifi here!!

const int port = 80;
bool stateLED = 0;

WebServer server(port);

void handleRoot()
{
  Serial.println("handleRoot");
  server.send(200, "text/plain", "        Hello from ESP32!"
                                 "\n ### List path Available ###"
                                 "\n #    - [POST] /btn        #"
                                 "\n #    - [POST] /onLED      #"
                                 "\n #    - [POST] /offLED     #"
                                 "\n #    - [GET] /stateLED    #"
                                 "\n ###########################\n" +
                                     String(charPicture));
}

void handleBtn()
{
  server.send(200, "text/html", "<html><body><h1>Control LED</h1><p> PIN LED " + String(LED) + " - State: " + String((stateLED ? "ON" : "OFF")) + "</p><p><a href=btnClick><button class=button>Toggle LED</button></a></p></body></html>");
}

void handleButtonClick()
{
  stateLED = !stateLED;
  digitalWrite(LED, !stateLED);
  Serial.println("Button clicked");
  handleBtn();
}

void handleControlLED(bool state)
{
  Serial.println("ControlLED");
  stateLED = state;
  digitalWrite(LED, !stateLED);
  StaticJsonDocument<200> doc;
  doc["message"] = "success";
  doc["stateLED"] = (stateLED ? "ON" : "OFF");
  String respBody;
  serializeJson(doc, respBody);
  server.send(200, "application/json", respBody);
}

void handleGetStateLED()
{
  Serial.println("StateLED");
  StaticJsonDocument<200> doc;
  doc["message"] = "success";
  doc["stateLED"] = (stateLED ? "ON" : "OFF");
  String respBody;
  serializeJson(doc, respBody);
  server.send(200, "application/json", respBody);
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // my builtin LED active LOW <<-- if GIOP active HIGH please set initail value LOW

  //@ setting if what to static ip device <<--

  // IPAddress staticIP(192, 168, 1, 119);
  // IPAddress gateway(192, 168, 1, 1);
  // IPAddress subnet(255, 255, 255, 0);
  // IPAddress dns(8, 8, 8, 8);
  // WiFi.config(staticIP, gateway, subnet, dns);

  //@ setting if what to static ip device <<--

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32")) //@ config MDNS name here <<--
  {
    Serial.println("Error encountered while starting mDNS");
    return;
  }

  server.onNotFound(handleRoot);
  server.on("/", handleRoot);
  server.on("/btn", HTTP_GET, handleBtn);
  server.on("/btnClick", handleButtonClick);
  server.on("/onLED", HTTP_POST, []()
            { handleControlLED(true); });
  server.on("/offLED", HTTP_POST, []()
            { handleControlLED(false); });
  server.on("/stateLED", HTTP_GET, handleGetStateLED);

  server.begin();
  MDNS.addService("http", "tcp", port);
}

void loop()
{
  server.handleClient();
}