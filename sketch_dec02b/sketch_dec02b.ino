#include <DHT.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "Wi-fi_0800";
const char *password = "StankinSS2023";

const char *mqttServer = "82.146.60.95";
const int mqttPort = 1883;
const char *mqttUser = "admin1";
const char *mqttPassword = "@dm!N";
const char *mqttClientId = "898989";
const char *mqttTempTopic = "Temp/позитрон";
const char *mqttHumpTopic = "Hump/позитрон";

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Servo servoMotor;
int servoPin = 5;

int motionSensorPin = 4;
bool motionDetected = false;

int ledPin = 13;

WiFiClient espClient;
PubSubClient client(espClient);

void sendMQTTData(const char *topic, float data) {
  char message[50];
  dtostrf(data, 6, 2, message);
  client.publish(topic, message);
}


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Подключение к MQTT брокеру
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  sendMQTTData(mqttTempTopic, temperature);
  sendMQTTData(mqttHumpTopic, humidity);

  if (humidity > 80) {
    servoMotor.write(90);
  } else {
    servoMotor.write(0);
  }

  motionDetected = digitalRead(motionSensorPin);
  if (motionDetected && temperature > 30) {
    digitalWrite(ledPin, HIGH);
    servoMotor.write(180);
    delay(500);
    digitalWrite(ledPin, LOW);
    servoMotor.write(0);
  }

  delay(1000);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
