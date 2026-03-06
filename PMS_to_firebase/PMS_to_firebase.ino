#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "PMS.h"

// 1. ตั้งค่า WiFi และ Firebase
#define WIFI_SSID "@Kitti"
#define WIFI_PASSWORD "tonn7771"
#define API_KEY "AIzaSyCBcpblKurO84WIbUDjNo9zZwDRdmvSkcU"
#define DATABASE_URL "https://test-realtime-database-c09fb-default-rtdb.asia-southeast1.firebasedatabase.app"

// 2. ตั้งค่า Email/Password (ที่คุณแจ้งว่ามี UID แล้ว)
#define USER_EMAIL "ce.fit.ssru2@gmail.com"
#define USER_PASSWORD "1212312121"

// 3. กำหนดพินเซนเซอร์
#define RAIN_DIGITAL_PIN 14  // พินสำหรับ DO
#define RAIN_ANALOG_PIN  34  // พินสำหรับ AO (ถ้าจะใช้)

HardwareSerial pmsSerial(2);
PMS pms(pmsSerial);
PMS::DATA data;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

void setup() {
  Serial.begin(115200);
  
  // ตั้งค่าพินเซนเซอร์น้ำฝน
  pinMode(RAIN_DIGITAL_PIN, INPUT);

  // เริ่มต้น PMS5003
  pmsSerial.begin(9600, SERIAL_8N1, 16, 17);

  // เชื่อมต่อ WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nConnected!");

  // ตั้งค่า Firebase Auth
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (pms.read(data)) {
    if (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0) {
      if (Firebase.ready()) {
        sendDataPrevMillis = millis();

        String uid = auth.token.uid.c_str();
        String userPath = "/Users/" + uid + "/Environment";

        // อ่านค่าจากเซนเซอร์น้ำฝน
        int isRainingRaw = digitalRead(RAIN_DIGITAL_PIN);
        int rainAnalog = analogRead(RAIN_ANALOG_PIN);
        bool isRaining = (isRainingRaw == LOW); // LOW แปลว่าฝนตก

        // เตรียมข้อมูลส่ง Firebase
        FirebaseJson json;
        json.set("AirQuality/PM25", data.PM_AE_UG_2_5);
        json.set("AirQuality/PM10", data.PM_AE_UG_10_0);
        json.set("Weather/isRaining", isRaining);
        json.set("Weather/rainValue", rainAnalog);
        json.set("last_update/.sv", "timestamp");

        if (Firebase.RTDB.updateNode(&fbdo, userPath, &json)) {
          Serial.printf("Data Sent! Rain: %s | PM2.5: %d\n", isRaining ? "YES" : "NO", data.PM_AE_UG_2_5);
        } else {
          Serial.println("Error: " + fbdo.errorReason());
        }
      }
    }
  }
}