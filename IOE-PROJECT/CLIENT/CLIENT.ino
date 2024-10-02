#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial mySerial(RX, TX);

long duration;
int distance[5], minDistance, checkDuration = 5;
int trigPin[5], echoPin[5];
int carStill[5];
int shareFlag = 0;

void setup() {
  // put your setup code here, to run once:

  trigPin[0] = 16;
  echoPin[0] = 5;
  trigPin[1] = 4;
  echoPin[1] = 0;
  trigPin[2] = 14;
  echoPin[2] = 12;
  trigPin[3] = 13;
  echoPin[3] = 15;

  pinMode(trigPin[0], OUTPUT);
  pinMode(echoPin[0], INPUT);
  pinMode(trigPin[1], OUTPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[2], INPUT);
  pinMode(trigPin[3], OUTPUT);
  pinMode(echoPin[3], INPUT);
  Serial.begin(115200);
  mySerial.begin(9600);

  Serial.println("Transmitter Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 4; i++) {
    // Clears the trigPin
    digitalWrite(trigPin[i], LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin[i], LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin[i], HIGH);

    // Calculating the distance
    distance[i] = duration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance[i]);
    if (distance[i] <= minDistance) {
      carStill[i] = checkTraffic(i);
    }
    shareFlag *= 10;
    shareFlag += carStill[i]; 
    delay(1000);
  }
  mySerial.println(shareFlag);  // Send message
  Serial.println("Message sent");
  delay(1000);
}

int checkTraffic(int i) {
  int j;
  while (j < checkDuration) {
    // Clears the trigPin
    digitalWrite(trigPin[i], LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin[i], LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin[i], HIGH);

    // Calculating the distance
    distance[i] = duration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance[i]);
    if (distance[i] > minDistance) {
      return 0;
    }
    delay(1000);
    j++;
  }
  return 1;
}