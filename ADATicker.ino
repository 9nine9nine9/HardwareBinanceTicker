#include <Arduino.h>
#include <heltec.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

#include <jsonlib.h>

//#include "adaIcon.h"  -- doesn't render properly rn :(


// User Config
#define SSID     "ssid as a string"  // CHANGE ME
#define PASSWORD "password as a string"  // CHANGE ME
#define UPDATE_T 30  // In seconds

// You can change the symbol string to something else if you like
#define ENDPOINT F("https://api.binance.com/api/v3/ticker/24hr?symbol=ADABUSD")

#include "rootCACert.h"


// Return human-formatted datestring
String getTime() {
  struct tm timeinfo;
  time_t curTime = time(nullptr);
  gmtime_r(&curTime, &timeinfo);
  return asctime(&timeinfo);
}

// WifiClientSecure doesn't seem to actually check if the cert is in a valid timerange.
// However, having current time is still nice for logging etc
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  Serial.print(F("Current time: "));
  Serial.println(getTime());
}


// Helper functions to handle displaying stuff in a uniform style
void displayLines(String line1, String line2) {
  Heltec.display->clear();
  Heltec.display->drawString(0, 0,  line1);
  Heltec.display->drawString(0, 24, line2);
  Heltec.display->display();
}

void displayStatus(String status) {
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 64-10, status);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->display();
}

void displayProgress(int progress) {
  Heltec.display->drawProgressBar(0, 64-10, 127, 9, progress);
  Heltec.display->display();
}

/*
void drawIcon() {
  Heltec.display->drawXbm((128-adaIconWidth), 0, adaIconWidth, adaIconHeight, adaIcon);
  Heltec.display->display();
}
*/


WiFiMulti WiFiMulti;

void setup() {
  // Initialise display
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->setContrast(255);

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  
  displayLines(F("Connecting"), F("to WiFi..."));
  displayStatus(SSID);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);

  // Wait for WiFi connection
  Serial.print(F("Waiting for WiFi to connect... "));
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(F("connected!"));

  displayLines(F("Getting"), F("the time..."));
  displayStatus(F("Requesting NTP time sync"));
  setClock();  
}

void loop() {  
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;

      displayLines(F("Loading"), F("prices..."));
      displayStatus(getTime());
      Serial.println(getTime());
  
      Serial.print(F("[HTTPS] begin...\r\n"));
      if (https.begin(*client, ENDPOINT)) {  // HTTPS
        Serial.print(F("[HTTPS] GET... "));
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("code: %d\r\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK) {
            String payload = https.getString();

            // Extract the json keys we care about (price and change)
            float priceChangePercent = jsonExtract(payload, "priceChangePercent").toFloat();
            float weightedAvgPrice   = jsonExtract(payload, "weightedAvgPrice").toFloat();

            Serial.printf("change: %.2f%%\r\nprice: $%.2f\r\n", priceChangePercent, weightedAvgPrice);
            displayLines("$"+String(weightedAvgPrice, 6), String(priceChangePercent, 6)+"%");
            
            //drawIcon();
            
            Serial.println(F("completed request"));
          }
        } else {
          String errCode = https.errorToString(httpCode).c_str();
          Serial.printf("[HTTPS] GET... failed, error: %s\r\n", errCode);

          displayLines(F("HTTPERR:"), errCode);
          displayStatus(F("Connect to serial for more info"));
          delay(10000);
        }
  
        https.end();
      } else {
        Serial.println(F("[HTTPS] Unable to connect"));

        displayLines(F("Couldn't"), F("connect"));
        displayStatus(F("Connect to serial for more info"));
        delay(10000);
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println(F("Unable to create WiFi client"));

    displayLines(F("Critical"), F("error!"));
    displayStatus(F("Couldn't create WiFi client!"));
    delay(10000);
  }

  //displayStatus(getTime());
  Serial.println();

  // Delay for UPDATE_T secs to keep network usage low
  for (int i=0; i <= UPDATE_T-1; i++) {
    displayProgress((int) (i*100.0/(UPDATE_T-1)));
    delay(1000);
  }
}
