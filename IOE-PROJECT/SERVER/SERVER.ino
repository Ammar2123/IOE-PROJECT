#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid =  "Friday";
const char *pass =  "12345678";

#define signalA D3
#define signalB D2
#define signalC D0
#define signalD D5
#define triggerA D6
#define triggerB D7
#define triggerC D8
#define triggerD D1

int densityA = 0, densityB = 0, densityC = 0, densityD = 0;
int tACount = 0, tBCount = 0, tCCount = 0, tDCount = 0, tCount = 0;
String carCount;

String server = "http://iracloud.yantravyuh.com/smartSignal/";
String update_car_count = server + "update_car_count.php?carCount=" + carCount;
String fetch_density = server + "fetch_density.php";


WiFiClient client;
HTTPClient http;

void setup() {
  // put your setup code here, to run once:
  pinMode(signalA, OUTPUT);
  pinMode(signalB, OUTPUT);
  pinMode(signalC, OUTPUT);
  pinMode(signalD, OUTPUT);
  pinMode(triggerA, INPUT);
  pinMode(triggerB, INPUT);
  pinMode(triggerC, INPUT);
  pinMode(triggerD, INPUT);
  Serial.begin(115200);   // Debugging via Serial Monitor

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  http.begin(client, fetch_density);
  int httpCode = http.GET();
  String Rec = http.getString();

  Serial.println("Received: " + Rec);
  int R = Rec.toInt();
  densityD = R % 10;
  R /= 10;
  densityC = R % 10;
  R /= 10;
  densityB = R % 10;
  R /= 10;
  densityA = R % 10;
  
  int trafficStat = checkStatus();
  if (trafficStat < 3 && trafficStat != 0) {
    if (densityA || densityC) {
      Serial.println("Executing AC for 60 sec");
      executeAC(trafficStat, 60);
    }
    else {
      Serial.println("Executing AC for 30 sec");
      executeAC(trafficStat, 30);
    }
  }
  else if (trafficStat >= 3) {
    if (densityB || densityD) {
      Serial.println("Executing BD for 60 sec");
      executeBD(trafficStat, 60);
    }
    else {
      Serial.println("Executing BD for 30 sec");
      executeBD(trafficStat, 30);
    }
  }
  else {
    Serial.println("Executing All Empty");
    allEmpty();
  }
  carCount = String(tACount) + "," + String(tBCount) + "," + String(tCCount) + "," + String(tDCount);

  // --------------------------------------------------------------- HTTP CALL START ---------------------------------------------------------------
  http.begin(client, update_car_count);
  httpCode = http.GET();
  String responseD = http.getString();
  // --------------------------------------------------------------- HTTP CALL END ---------------------------------------------------------------

}

int checkStatus() {
  int tA = digitalRead(triggerA);
  int tB = digitalRead(triggerB);
  int tC = digitalRead(triggerC);
  int tD = digitalRead(triggerD);

  if (tA == 0 && tB == 0 && tC == 0 && tD == 0)
    return 0;
  else if (tA) {
    tACount++;
    tCount++;
    return 1;
  }
  else if (tB) {
    tBCount++;
    tCount++;
    return 2;
  }
  else if (tC) {
    tCCount++;
    tCount++;
    return 3;
  }
  else if (tD) {
    tDCount++;
    tCount++;
    return 4;
  }
  return 0;
}

int allEmpty() {
  Serial.println("Executing AC for 15 sec");
  executeAC(1, 15);
  Serial.println("Executing BD for 15 sec");
  executeBD(3, 15);
  return 0;
}

int executeAC(int signalPr, int timeD) {
  double delayRight = (timeD * 1000) / 3;
  double delayStraight = (2000 * timeD) / 3;
  switch (signalPr) {
    case 1:
      Serial.println("Executing AC for A Is priority");
      digitalWrite(signalA, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalC, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalA, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalC, LOW); //GO Right
      break;
    case 2:
      Serial.println("Executing AC for C Is priority");
      digitalWrite(signalC, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalA, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalC, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalA, LOW); //GO Right
      break;
  }
  delay(1000);
  return 0;
}
int executeBD(int signalPr, int timeD) {
  double delayRight = (timeD * 1000) / 3;
  double delayStraight = (2000 * timeD) / 3;
  switch (signalPr) {
    case 3:
      Serial.println("Executing BD for B Is priority");
      digitalWrite(signalB, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalD, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalB, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalD, LOW); //GO Right
      break;
    case 4:
      Serial.println("Executing BD for D Is priority");
      digitalWrite(signalD, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalB, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalD, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalB, LOW); //GO Right
      break;
  }
  delay(1000);
  return 0;
}
