
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP8266
 *
 * Copyright (c) 2023 mobizt
 *
 */

/** This example will show how to authenticate using
 * the legacy token or database secret with the new APIs (using config and auth data).
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Provide environment variables used in the WIFI and FIREBASE connection.
#include "config.h"

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID SSID
#define WIFI_PASSWORD PASSWORD

/* 2. If work with RTDB, define the RTDB URL and database secret */
#define DATABASE_URL URL //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET SECRET

/* Define ports */
#define LED_RED_PIN 5
#define LED_YELLOW_PIN 18
#define ECHO_PIN 2
#define TRIG_PIN 4

#define SOUND_SPEED 0.034
#define TIME_ZONE -3

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

long duration;
float distance;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TIME_ZONE * 3600, 60000);

void activateAlarm();
String getCurrentDate();
void getDistance();

void setup()
{
    /* Mapping Ports */
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    //pinMode(POT_PIN, INPUT);
    pinMode(ECHO_PIN, INPUT);

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the certificate file (optional) */
    // config.cert.file = "/cert.cer";
    // config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL and database secret(required) */
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
    Firebase.reconnectNetwork(true);

    // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
    // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    // Or use legacy authenticate method
    // Firebase.begin(DATABASE_URL, DATABASE_SECRET);

    // Initializing NTP client
    timeClient.begin();
    timeClient.update();
}

void loop()
{
    String currentDate = getCurrentDate();
    Serial.println("Alarm [OFF]");
    getDistance();

    while (distance < 5.00 && distance != 0)
    {
        Serial.println("Alarm [ON]");
        Serial.println(currentDate);
        Serial.printf("Set state... %s\n", Firebase.setBool(fbdo, "/alarm/state", true) ? "ok" : fbdo.errorReason().c_str());
        Serial.printf("Set last time activated... %s\n", Firebase.setString(fbdo, "/alarm/lastTimeActivated/", currentDate) ? "ok" : fbdo.errorReason().c_str());

        activateAlarm();
        getDistance();

        if (distance > 5.00)
            Serial.printf("Set state... %s\n", Firebase.setBool(fbdo, "/alarm/state", false) ? "ok" : fbdo.errorReason().c_str());
    }

    while (Firebase.get(fbdo, "/alarm/state/") && fbdo.boolData() == true)
    {
        Serial.println("Alarm [ON]");
        Firebase.get(fbdo, "/alarm/lastTimeActivated/");
        Serial.println(fbdo.boolData());

        activateAlarm();
    }

    delay(500);
}

void activateAlarm()
{
    //float intensity = map(analogRead(POT_PIN), 0, 1023, 0, 255);
    //analogWrite(LED_RED_PIN, intensity);
    //analogWrite(LED_YELLOW_PIN, intensity);

    digitalWrite(LED_RED_PIN, true);
    digitalWrite(LED_YELLOW_PIN, false);
    delay(500);
    digitalWrite(LED_RED_PIN, false);
    digitalWrite(LED_YELLOW_PIN, true);
    delay(500);
}

String getCurrentDate()
{
    timeClient.update();
    time_t rawTime = timeClient.getEpochTime();
    struct tm *timeInfo = localtime(&rawTime);

    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %s", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, timeClient.getFormattedTime().c_str());

    return String(buffer);
}

void getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * SOUND_SPEED / 2;
    Serial.printf("Distance = %fcm\n", distance);
}
