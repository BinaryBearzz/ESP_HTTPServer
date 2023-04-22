#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

const char *ssid = "ABC_2.4G";
const char *password = "Ais@auth";

WebServer server(80);

void handleRoot()
{
  server.send(200, "text/plain", "Hello from ESP32!");
}

void handleGet()
{
  server.send(200, "text/plain", "This is a GET request");
}

void handlePost()
{
  String body = server.arg("plain");

  // Parse the JSON payload
  StaticJsonDocument<200> doc;
  deserializeJson(doc, body);

  // Get the values from the JSON payload
  String name = doc["name"];
  int age = doc["age"];

  // Do something with the values
  Serial.print("Name: ");
  Serial.println(name);
  Serial.print("Age: ");
  Serial.println(age);
  server.send(200, "text/plain", "This is a POST request");
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  IPAddress staticIP(192, 168, 1, 119);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);

  WiFi.config(staticIP, gateway, subnet, dns);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32"))
  {
    Serial.println("Error starting mDNS");
    return;
  }

  server.on("/", handleRoot);
  server.on("/get", handleGet);
  server.on("/post", HTTP_POST, handlePost);

  server.begin();

  // if (MDNS.begin("lamp"))
  // { // Start the mDNS responder for esp8266.local
  //   Serial.println("MDNS responder started ");
  // }
  // MDNS.addService("http", "tcp", 80);
}

void loop()
{
  // MDNS.queryService("http", "tcp");
  server.handleClient();
}