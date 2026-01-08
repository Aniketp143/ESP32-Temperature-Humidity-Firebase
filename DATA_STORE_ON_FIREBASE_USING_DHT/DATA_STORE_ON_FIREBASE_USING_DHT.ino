#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

/* ---------------- WIFI CREDENTIALS ---------------- */
#define WIFI_SSID "Aniket"
#define WIFI_PASSWORD "00000000"

/* ---------------- FIREBASE CREDENTIALS ---------------- */
#define API_KEY "AIzaSyDqIqOJ2UcH7kF0BH9eyARFRAFA4Nou6oA"
#define DATABASE_URL "https://temp-humidity-monitoring-f0d02-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "fC49kPjBhys4vCl0SMroSOVtRwOJGN9ssZ7Vffvx"  // Legacy token

/* ---------------- DHT CONFIGURATION ---------------- */
#define DHTPIN 4
#define DHTTYPE DHT11   // Change to DHT22 if needed

DHT dht(DHTPIN, DHTTYPE);

/* ---------------- FIREBASE OBJECTS ---------------- */
FirebaseData fbdo;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  /* -------- WIFI CONNECTION -------- */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected");

  /* -------- FIREBASE CONFIG WITH DATABASE SECRET -------- */
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, nullptr);  // no auth object needed
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Ready");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT Sensor Error");
    delay(2000);
    return;
  }

  FirebaseJson json;
  json.set("temperature", temperature);
  json.set("humidity", humidity);

  if (Firebase.RTDB.pushJSON(&fbdo, "/TempHumidityData/readings", &json)) {
    Serial.println("✅ Data uploaded successfully");
  } else {
    Serial.print("❌ Firebase Error: ");
    Serial.println(fbdo.errorReason());
  }

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(5000);  // Upload every 5 seconds
}
