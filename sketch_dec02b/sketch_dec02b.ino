#include <DHT.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Параметры Wi-Fi
const char *ssid = "Wi-fi_0800";
const char *password = "StankinSS2023";

// Параметры MQTT
const char *mqttServer = "82.146.60.95";
const int mqttPort = 1883;
const char *mqttUser = "admin1";
const char *mqttPassword = "@dm!N";
const char *mqttClientId = "898989";
const char *mqttTempTopic = "Temp/позитрон";
const char *mqttHumpTopic = "Hump/позитрон";

// Параметры датчика температуры и влажности (DHT)
#define DHTPIN 2 // Пин подключения датчика DHT 
#define DHTTYPE DHT11 // Выберите тип датчика DHT (DHT11 или DHT22) 
DHT dht(DHTPIN, DHTTYPE);

// Параметры сервомотора
Servo servoMotor;
int servoPin = 5; // Пин подключения сервомотора

// Параметры датчика движения
int motionSensorPin = 4; // Пин подключения датчика движения
bool motionDetected = false;

// Параметры светодиода
int ledPin = 13; // Пин подключения светодиода

// Инициализация Wi-Fi и MQTT клиента
WiFiClient espClient;
PubSubClient client(espClient);

// Функция для отправки данных по MQTT
void sendMQTTData(const char *topic, float data) {
  char message[50];
  dtostrf(data, 6, 2, message);
  client.publish(topic, message);
}


void setup() {
  Serial.begin(115200);

  // Подключение к Wi-Fi
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
  // Считывание данных с датчика температуры и влажности
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Отправка данных по MQTT
  sendMQTTData(mqttTempTopic, temperature);
  sendMQTTData(mqttHumpTopic, humidity);

  // Проверка условий для управления сервомотором
  if (humidity > 80) {
    servoMotor.write(90); // Открыть дверцу
  } else {
    servoMotor.write(0); // Закрыть дверцу
  }

  // Проверка движения и температуры для управления светодиодом и сервомотором
  motionDetected = digitalRead(motionSensorPin);
  if (motionDetected && temperature > 30) {
    digitalWrite(ledPin, HIGH); // Зажечь светодиод
    servoMotor.write(180); // Повернуть сервомотор
    delay(500);
    digitalWrite(ledPin, LOW); // Погасить светодиод
    servoMotor.write(0); // Вернуть сервомотор в исходное положение
  }

  delay(1000); // Пауза 1 секунда

  // Подключение к MQTT брокеру, если не подключено
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Функция для переподключения к MQTT брокеру
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
