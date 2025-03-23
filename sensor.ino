#include <ESP8266WiFi.h>
#include <DHT.h>
#include <FirebaseESP8266.h>
#include "HX710B.h"

// HX710B pressure sensor pins
// #define SCK_PIN 3
// #define SDI_PIN 4
// HX710B air_press(SCK_PIN, SDI_PIN);

// DHT Sensor setup
#define DHTPIN 2         // GPIO2 (D4 on most boards)
#define DHTTYPE DHT11    // Change to DHT22 if using that sensor

DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "POCO";         // Replace with your Wi-Fi SSID
const char* password = "Helloworld"; // Replace with your Wi-Fi password
