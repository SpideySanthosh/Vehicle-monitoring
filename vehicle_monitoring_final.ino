#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <ESP8266WiFi.h>
#include "DHT.h"

// WiFi parameters
const char* ssid = "ssid";
const char* password = "password";

// Pin
#define DHTPIN 14

// Use DHT11 sensor
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// Host
const char* host = "dweet.io";
char* smokedetect = "no";

int buzzer = 12;
int smokeA0 = A0;
// Your threshold value
int sensorThres = 300;


Adafruit_BMP085 bmp;
  
void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);

  delay(10);

  // Init DHT
  dht.begin();
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(10);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}
  
void loop() {
  
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
    
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Humidity = ");
  Serial.print(dht.readHumidity());
  Serial.println(" rh");
  int analogSensor = analogRead(smokeA0);

  Serial.print("Smoke ");
  Serial.println(analogSensor);

  if (analogSensor > sensorThres)
  {
    digitalWrite(buzzer,HIGH);
    smokedetect = "yes";
  }
  else
  {
    digitalWrite(buzzer,LOW);
    smokedetect = "no";
  }
    

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  delay(10);

  // Reading temperature and humidity
  int h = dht.readHumidity();
  delay(10);
  // Read temperature as Celsius
  int t = bmp.readTemperature();
  delay(10);

  int p = bmp.readPressure();
  delay(10);

  // This will send the request to the server
  client.print(String("GET /dweet/for/vehiclemonitoring?temperature=") + String(t) + "&humidity=" + String(h) + "&pressure=" + String(p) + "&firethreat=" + String(smokedetect) + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  // Repeat every 10 seconds
  delay(5000);
}

