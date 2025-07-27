#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32_MailClient.h>
#include <DHT.h>
#include <SPIFFS.h>
#include <ESP32Servo.h>  // ESP32-specific Servo library

// Sensor and GPIO Pins
#define RAIN_SENSOR_PIN 5
#define ANALOG_PIN 35
#define FLAME_SENSOR_PIN 34
#define VIBRATION_SENSOR_PIN 33
#define MOISTURE_SENSOR_PIN 32
#define DHTPIN 27
#define DHTTYPE DHT11
#define SERVO_PIN 26
#define BUZZER 13

// Relay Pins
const int relaypin1 = 15;
const int relaypin2 = 2;
const int relaypin3 = 4;
const int relaypin4 = 16;

// WiFi Credentials
const char* ssid = "Vicky";
const char* pass = "Vikash@2006";

// Email Configuration
#define emailSenderAccount    "homemoniteringsystem@gmail.com"
#define emailSenderPassword   "dsivznmdtvpcazjp"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465

#define emailSubjectGas       "ALERT! GAS LEAKED"
#define emailSubjectFire      "ALERT! FIRE DETECTED"
#define emailSubjectVibration "ALERT! VIBRATION DETECTED"
#define emailSubjectRain      "ALERT! RAIN DETECTED"

// Global Variables
DHT dht(DHTPIN, DHTTYPE);
Servo servo;
AsyncWebServer server(80);

bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;
bool relayState4 = false;

bool gasDetected = false;
bool fireDetected = false;
bool vibrationDetected = false;
bool lowmoisture = false;
bool rainDetected = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  
  // GPIO Setup
  pinMode(ANALOG_PIN, INPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  dht.begin();
  
  pinMode(relaypin1, OUTPUT);
  pinMode(relaypin2, OUTPUT);
  pinMode(relaypin3, OUTPUT);
  pinMode(relaypin4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(0);

  digitalWrite(relaypin1, HIGH);
  digitalWrite(relaypin2, HIGH);
  digitalWrite(relaypin3, HIGH);
  digitalWrite(relaypin4, HIGH);
  digitalWrite(BUZZER, LOW);

  // SPIFFS Initialization
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // WiFi Connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Web Server Routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("relay")) {
      int relay = request->getParam("relay")->value().toInt();
      switch (relay) {
        case 1:
          relayState1 = !relayState1;
          digitalWrite(relaypin1, relayState1 ? LOW : HIGH);
          break;
        case 2:
          relayState2 = !relayState2;
          digitalWrite(relaypin2, relayState2 ? LOW : HIGH);
          break;
        case 3:
          relayState3 = !relayState3;
          digitalWrite(relaypin3, relayState3 ? LOW : HIGH);
          break;
        case 4:
          relayState4 = !relayState4;
          digitalWrite(relaypin4, relayState4 ? LOW : HIGH);
          break;
      }
      request->send(200, "text/plain", "Relay Toggled");
    }
  });

  server.on("/sensorValues", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int moisture = analogRead(MOISTURE_SENSOR_PIN);
    int gasValue = analogRead(ANALOG_PIN);
    int flameValue = digitalRead(FLAME_SENSOR_PIN);
    int vibrationValue = digitalRead(VIBRATION_SENSOR_PIN);
    int rainValue = digitalRead(RAIN_SENSOR_PIN);

    gasDetected = gasValue > 3000;
    fireDetected = flameValue == LOW;
    vibrationDetected = vibrationValue == HIGH;
    rainDetected = rainValue == LOW;
    lowmoisture = moisture < 3000;

    String jsonResponse = "{";
    jsonResponse += "\"temperature\": " + String(temperature) + ",";
    jsonResponse += "\"humidity\": " + String(humidity) + ",";
    jsonResponse += "\"moisture\": " + String(moisture) + ",";
    jsonResponse += "\"gasSensorStatus\": " + String(gasDetected) + ",";
    jsonResponse += "\"flameDetected\": " + String(fireDetected) + ",";
    jsonResponse += "\"vibrationStatus\": " + String(vibrationDetected) + ",";
    jsonResponse += "\"rainStatus\": " + String(rainDetected);
    jsonResponse += "}";

    request->send(200, "application/json", jsonResponse);
  });

  server.begin();
}

void loop() {
  int gasValue = analogRead(ANALOG_PIN);
  int flameValue = digitalRead(FLAME_SENSOR_PIN);
  int vibrationValue = digitalRead(VIBRATION_SENSOR_PIN);
  int rainValue = digitalRead(RAIN_SENSOR_PIN);
  int moisture = analogRead(MOISTURE_SENSOR_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  gasDetected = gasValue > 3000;
  fireDetected = flameValue == LOW;
  vibrationDetected = vibrationValue == HIGH;
  rainDetected = rainValue == LOW;
  lowmoisture = moisture < 3000;

  Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
          Serial.print("Moisture :");
    Serial.println(moisture);

  if (gasDetected) {
    digitalWrite(relaypin1, HIGH);
    digitalWrite(relaypin2, HIGH);
    digitalWrite(relaypin3, HIGH);
    digitalWrite(BUZZER, HIGH);
    servo.write(90);
        Serial.println(gasValue);
    Serial.println("Gas detected!");
    sendEmailNotification(emailSubjectGas, "GAS detected! Check the Kitchen area.");
  } else {
    servo.write(0);
    digitalWrite(BUZZER, LOW);
  }

  if (fireDetected) {
        Serial.print("flame :");
     Serial.println(flameValue);
    Serial.println("Fire detected!");
    digitalWrite(BUZZER, HIGH);
    sendEmailNotification(emailSubjectFire, "FIRE detected! Immediate action required.");
  }

  if (vibrationDetected) {
        Serial.print("vibration :");
    Serial.println(vibrationValue);
    Serial.println("Vibration detected!");
    sendEmailNotification(emailSubjectVibration, "VIBRATION detected! Check for possible movements or disturbances.");
  }

  if (rainDetected) {
    Serial.print("Rain :");
    Serial.println(rainValue);
    Serial.println("Rain detected!");
    sendEmailNotification(emailSubjectRain, "Rain detected! Take necessary precautions.");
  }

  if (lowmoisture) {
    digitalWrite(relaypin4, LOW);
    Serial.print("Moisture :");
    Serial.println(moisture);
    Serial.println("PUMP ON");
  } else {
    digitalWrite(relaypin4, HIGH);
  }

  delay(1000);
}

void sendEmailNotification(const char* subject, const char* message) {
  SMTPData smtpData;
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  smtpData.setSender("ESP32_ALERT", emailSenderAccount);
  smtpData.setPriority("High");
  smtpData.setSubject(subject);
  smtpData.setMessage(message, true);
  smtpData.addRecipient("karrothuvikas2006@gmail.com");

  if (!MailClient.sendMail(smtpData)) {
    Serial.println("Error sending Email: " + MailClient.smtpErrorReason());
  } else {
    Serial.println("Email sent successfully");
  }
  smtpData.empty();
}
