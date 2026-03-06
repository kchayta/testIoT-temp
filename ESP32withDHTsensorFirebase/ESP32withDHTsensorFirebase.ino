#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

/* WiFi */
#define WIFI_SSID "ROBOT-05"
#define WIFI_PASSWORD "1212312121"

/* Firebase */
#define API_KEY "AIzaSyDnV0IikMb-244PYra66bHeFbs0NN4NxN4"
#define DATABASE_URL "ssrudatabase-default-rtdb.asia-southeast1.firebasedatabase.app"

#define USER_EMAIL "YOUR Email"
#define USER_PASSWORD "YOUR PASSWORD"

/* DHT Sensor */
#define DHTPIN 14
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

/* Firebase objects */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

void setup()
{
  Serial.begin(115200);

  /* DHT start */
  dht.begin();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  /* WiFi connect */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  /* Firebase config */
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);

  Serial.println("Firebase Ready");
}

void loop()
{
  if (Firebase.ready() && millis() - sendDataPrevMillis > 5000)
  {
    sendDataPrevMillis = millis();

    sensors_event_t event;

    float temperature = NAN;
    float humidity = NAN;

    /* Read Temperature */
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature))
    {
      temperature = event.temperature;
    }

    /* Read Humidity */
    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity))
    {
      humidity = event.relative_humidity;
    }

    Serial.print("Temp: ");
    Serial.println(temperature);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    /* Send to Firebase */

    if (Firebase.setFloat(fbdo, "/sensor/temperature", temperature))
      Serial.println("Temperature sent");
    else
      Serial.println(fbdo.errorReason());

    if (Firebase.setFloat(fbdo, "/sensor/humidity", humidity))
      Serial.println("Humidity sent");
    else
      Serial.println(fbdo.errorReason());

    Serial.println("--------------------");
  }
}
