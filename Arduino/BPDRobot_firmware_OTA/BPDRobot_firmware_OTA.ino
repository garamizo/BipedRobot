#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP32Servo.h>

#define ssid ""
#define password ""
#include "credentials.h" // overwrite w/ my ssid, password

//variabls for blinking an LED with Millis
const int led = LED_BUILTIN; // ESP32 Pin to which onboard LED is connected
int servoPin1 = 18, servoPin2 = 22;

unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 1000;  // interval at which to blink (milliseconds)
int ledState = LOW;  // ledState used to set the LED
Servo myservo1, myservo2;

void setup() {

  pinMode(led, OUTPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo1.setPeriodHertz(50);    // standard 50 hz servo
  myservo1.attach(servoPin1, 1000, 2000); // attaches the servo on pin 18 to the servo object
  myservo2.setPeriodHertz(50);    // standard 50 hz servo
  myservo2.attach(servoPin2, 1000, 2000); 
  
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();  
  
  //loop to blink without delay
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ledState = not(ledState);
    digitalWrite(led,  ledState);
  }

  static long t2 = millis();
  if (millis() - t2 > 15) {
    t2 = millis();

    const float freq = 2 * PI / 4;  // rad
    float t = millis() / 1e3;

    float pos1 = 80 + 40 * sin(t * freq);
    float pos2 = 90 + 50 * cos(t * freq);

    pos1 = constrain(round(pos1), 30, 150);
    pos2 = constrain(round(pos2), 30, 150);

    myservo1.write(pos1);
    myservo2.write(pos2);
  }

}
