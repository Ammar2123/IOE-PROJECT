#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid =  "Friday";
const char *pass =  "12345678";

long duration;
int distance[5], minDistance = 100, checkDuration = 5;
int trigPin[5], echoPin[5];
int carStill[5], jamCounter[5];
int shareFlag;
String jC;

String server = "http://iracloud.yantravyuh.com/smartSignal/";
String update_density = server + "update_density.php?shareFlag=" + shareFlag;
String update_jam_count = server + "update_jam_count.php?jC=" + jC;


WiFiClient client;
HTTPClient http;

void setup() {
  // put your setup code here, to run once:

  trigPin[0] = 16; //D0
  echoPin[0] = 5; //D1
  trigPin[1] = 4; //D2
  echoPin[1] = 0; //D3
  trigPin[2] = 14; //D5
  echoPin[2] = 12; //D6
  trigPin[3] = 15; //D8
  echoPin[3] = 13; //D7

  pinMode(trigPin[0], OUTPUT);
  pinMode(echoPin[0], INPUT);
  pinMode(trigPin[1], OUTPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[2], INPUT);
  pinMode(trigPin[3], OUTPUT);
  pinMode(echoPin[3], INPUT);

  //  pinMode(D0, OUTPUT);
  //  pinMode(D1, INPUT);
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Transmitter Ready");
}

void loop() {
  shareFlag = 0;
  // put your main code here, to run repeatedly:
  Serial.println("Loop!!");
  delay(2000);
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
    Serial.print("Distance ");
    Serial.print(i);
    Serial.println(distance[i]);
    if (distance[i] <= minDistance) {
      carStill[i] = checkTraffic(i);
    }
    else
      carStill[i] = 2;
    shareFlag *= 10;
    shareFlag += carStill[i];
    delay(1000);
  }
  Serial.println(shareFlag);  // Send message
  jC = String(jamCounter[0]) + "," + String(jamCounter[1]) + "," + String(jamCounter[2]) + "," + String(jamCounter[3]);
  Serial.println(jC);
// --------------------------------------------------------------- HTTP CALL START ---------------------------------------------------------------
  http.begin(client, update_density);
  int httpCode = http.GET();
  String responseD = http.getString();

  http.begin(client, update_jam_count);
  httpCode = http.GET();
  String responseJ = http.getString();
// --------------------------------------------------------------- HTTP CALL END ---------------------------------------------------------------

  Serial.println("Density Server Message Rec: " + responseD);
  Serial.println("Jam Count Server Message Rec: " + responseJ);
  delay(1000);
}

int checkTraffic(int i) {
  int j = 0;
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
      return 2;
    }
    delay(1000);
    j++;
  }
  jamCounter[i]++;
  return 1;
}
