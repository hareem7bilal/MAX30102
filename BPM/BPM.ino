#define BLYNK_TEMPLATE_ID "TMPL7HGhh_2B"
#define BLYNK_DEVICE_NAME "MAX30102 BPM"
#define BLYNK_AUTH_TOKEN "6lTJdYR90xqI0ECPw-mQ1OlLDTBpZAWH"

#define LED_Y 4
#define LED_G 15
#define LED_R 2

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BlynkSimpleEsp32.h>

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

char auth[] = BLYNK_AUTH_TOKEN;             // You should get Auth Token in the Blynk App.
char ssid[] = "D41-6";                              // Your WiFi credentials.
char pass[] = "17171717";

//use right middle finger/right thumb
void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);


  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  while (Serial.available() == 0) ; //wait until user presses a key
  Serial.read();

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}



void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;

    }
  }
  Blynk.run();
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  Blynk.virtualWrite(V3, beatAvg);

  if (irValue < 50000)
    Serial.print("   No finger");
  Serial.println();


  if (beatAvg < 40) {
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_R, LOW);
  }

  else if (beatAvg > 100) {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
  }

  else {
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_Y, LOW);
  }


}
