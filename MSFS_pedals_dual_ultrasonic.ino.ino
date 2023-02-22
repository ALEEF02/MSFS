// Code for rudder to communicate with the main MSFS Panel

#include <StringSplitter.h>
#include <Wire.h>
#include <WString.h>

//Ultrasonic Sensor
#define echoPin1 3
#define trigPin1 2
#define echoPin2 5
#define trigPin2 4
#define LEDPin LED_BUILTIN // Onboard LED
int maxDist = 870;
int tempF = 81; // Set current room temp here
float tempK;
double speedSoundMetersPerMicrosecond;
long duration1;
long duration2;
double distance1; // Duration used to calculate distance
double distance2;

/* Ultrasonic env
 *  40 Hz
 *  83F
 *  8.703336993 m
 *  348.13347972 m/s
 */

void setup() {
  Wire.begin(10);
  Wire.setClock(10000);
  Wire.onRequest(requestDist);
  delay(100);
  //Serial.println("Booting");
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Use LED indicator (if required)
  digitalWrite(LED_BUILTIN, LOW);
  //Serial.println(String(tempF, 1) + " F");
  tempK = (((tempF - 32.0) * (5.0/9.0)) + 273.15);
  //Serial.println(String(tempK, 2) + " K");
  speedSoundMetersPerMicrosecond = sqrt((1.4 * 8.314 * tempK) / (0.02895)) / 1000000.0;
  //Serial.println(String(speedSoundMetersPerMicrosecond, 10) + " m/microsecond");
}

void loop() {
  /*
  * The following trigPin/echoPin cycle is used to determine the
  * distance of the nearest object by bouncing soundwaves off of it.
  */ 
  digitalWrite(trigPin1, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);

  digitalWrite(trigPin2, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound. (* 100 for no decimal)
  distance1 = speedSoundMetersPerMicrosecond * duration1 * 50.0 * 0.393701 * 100;
  distance2 = speedSoundMetersPerMicrosecond * duration2 * 50.0 * 0.393701 * 100;

  //Serial.println(String(distance1, 0) + ";" + String(distance2, 0));

  delay(250);
}

void requestDist() {
  digitalWrite(LED_BUILTIN, HIGH);
  String out = (String(distance1, 0) + String(distance2, 0));
  char swaws[6];
  strcpy(swaws, out.c_str());
  Wire.write(swaws);
  digitalWrite(LED_BUILTIN, LOW);
}
