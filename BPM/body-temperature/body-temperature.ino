#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit


void setup() {
  Serial.begin(115200); // initialize serial
  DS18B20.begin();    // initialize the DS18B20 sensor
}

void loop() {
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32; // convert °C to °F

  Serial.print("Temperature: ");
  Serial.print(tempC);    // print the temperature in °C
  Serial.print("°C");
  Serial.print("  ~  ");  // separator between °C and °F
  Serial.print(tempF);    // print the temperature in °F
  Serial.println("°F");

  delay(500);
}
