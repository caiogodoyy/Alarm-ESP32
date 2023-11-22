
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
#include <ctime>
#include <sstream>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "AP602-V2G"
#define WIFI_PASSWORD "mKu8fy5p"

/* 2. If work with RTDB, define the RTDB URL and database secret */
#define DATABASE_URL "https://alarm-esp32-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "BqimfdBYyYFa63e8pY7nJCD7hmbGWHdmbeMomXQO"

/* Define ports */
#define BIP_PIN 14
#define LED_RED_PIN 26
#define LED_YELLOW_PIN 25
#define ECHO_PIN 32
#define TRIGGER_PIN 35

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

void activateAlarm();
std::string getCurrentTime();

void setup()
{
    /* Mapping Ports */
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(BIP_PIN, OUTPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
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
}

void loop()
{
    Serial.println("Alarm OFF");
    getCurrentTime();

    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * SOUND_SPEED / 2;
    Serial.printf("Distance = %fcm", distance);

    while (distance < 2.00 && distance != 0)
    {
        Serial.println("Alarm ON");
        Serial.printf("Set state... %s\n", Firebase.setBool(fbdo, "/alarm/state", true) ? "ok" : fbdo.errorReason().c_str());
        Serial.printf("Set last time activated... %s\n", Firebase.setString(fbdo, "/alarm/lastTimeActivated/", getCurrentTime()) ? "ok" : fbdo.errorReason().c_str());

        activateAlarm();
    }

    Serial.printf("Set state... %s\n", Firebase.setBool(fbdo, "/alarm/state", false) ? "ok" : fbdo.errorReason().c_str());

    delay(1000);
}

void activateAlarm()
{
    digitalWrite(LED_RED_PIN, true);
    digitalWrite(LED_YELLOW_PIN, false);
    delay(500);
    digitalWrite(LED_RED_PIN, false);
    digitalWrite(LED_YELLOW_PIN, true);
    delay(500);
}

std::string getCurrentTime()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* tmStruct = std::localtime(&currentTime);

    std::stringstream ss;
    ss << (tmStruct->tm_year + 1900) << '-'
       << (tmStruct->tm_mon + 1) << '-'
       << tmStruct->tm_mday << ' '
       << tmStruct->tm_hour << ':'
       << tmStruct->tm_min << ':'
       << tmStruct->tm_sec;

    return ss.str();
}
