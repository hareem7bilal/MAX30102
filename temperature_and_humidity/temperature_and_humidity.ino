#define BLYNK_TEMPLATE_ID "TMPL6rwHHJf-"
#define BLYNK_DEVICE_NAME "temperature and humidity"
#define BLYNK_AUTH_TOKEN "E_VtvcbbfwJSJMrY22frlVhytPu53PSp"

#include <Wire.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include "MAX30105.h"


#define SENSOR_PIN  5 // ESP32 pin GIOP21 connected to DS18B20 sensor's DQ pin
#define DHTPIN 18   // ESP32 pin GIOP18 connected to DHT11 sensor's DQ pin
#define DHTTYPE DHT11
#define LED_Y 4
#define LED_G 15
#define LED_R 23

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
MAX30105 particleSensor;

float body_temp; // temperature in Celsius

DHT dht(DHTPIN, DHTTYPE);// Initialize DHT sensor for normal 16mhz Arduino

char auth[] = BLYNK_AUTH_TOKEN;             // You should get Auth Token in the Blynk App.
char ssid[] = "D41-6";                              // Your WiFi credentials.
char pass[] = "17171717";

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  Blynk.begin(auth, ssid, pass);
   
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  // Initialize sensor
  if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false) { //Use default I2C port, 400kHz speed
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }

  //The LEDs are very low power and won't affect the temp reading much but
  //you may want to turn off the LEDs to avoid any local heating
  particleSensor.setup(0); //Configure sensor. Turn off LEDs
  particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.
  dht.begin();
 

}

void loop() {
 
  float MAX30102_room_temp = particleSensor.readTemperature();

  DS18B20.requestTemperatures();       // send the command to get temperatures
  body_temp = DS18B20.getTempCByIndex(0);  // read temperature in °C

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius
  float DHT11_room_temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(DHT11_room_temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (body_temp < 35) {
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_R, LOW);
  }


  else if (body_temp > 38) {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
  }


  else {
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_Y, LOW);
  }

  Serial.print(" MAX30102 room temperature: ");
  Serial.print(MAX30102_room_temp, 2);
  Serial.print("°C");
 
 
  Blynk.run();

  Serial.print(", DHT11 room temperature: ");
  Serial.print(DHT11_room_temperature, 2);
  Serial.print("°C");
  Blynk.virtualWrite(V1, DHT11_room_temperature);

  Serial.print(", Body temperature: ");
  Serial.print(body_temp, 2);   // print the temperature in °C
  Serial.print("°C");
  Blynk.virtualWrite(V2, body_temp);

  Serial.print(", Humidity: ");
  Serial.print(humidity);
  Serial.print("%");
  Blynk.virtualWrite(V3, humidity);



  Serial.println();
}
