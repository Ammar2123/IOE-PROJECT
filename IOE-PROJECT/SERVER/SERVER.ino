#include <SoftwareSerial.h>

SoftwareSerial mySerial(RX, TX);

#define signalA D0
#define signalB D1
#define signalC D2
#define signalD D3
#define triggerA D5
#define triggerB D6
#define triggerC D7
#define triggerD D8

int densityA = 0, densityB = 0, densityC = 0, densityD = 0;
int tACount = 0, tBCount = 0, tCCount = 0, tDCount = 0, tCount = 0;
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
  mySerial.begin(9600);   // SoftwareSerial at 9600 baud rate
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mySerial.available()) {          // Check if data is available
    String received = mySerial.readString();  // Read incoming data
    Serial.print("Received: ");
    int R = received.toInt();
    densityD = R % 10;
    R /= 10;
    densityC = R % 10;
    R /= 10;
    densityB = R % 10;
    R /= 10;
    densityA = R % 10;
    Serial.println(R);  // Print the received message
  }
  int trafficStat = checkStatus();
  if(trafficStat < 3){
    if(densityA || densityC)
      executeAC(trafficStat, 60);
    else
      executeAC(trafficStat, 30);
    }
  else if(trafficStat >= 3){
    if(densityB || densityD)
      executeBD(trafficStat, 60);
    else
      executeBD(trafficStat, 30);
    }
}

int checkStatus(){
  int tA = digitalRead(triggerA);
  int tB = digitalRead(triggerB);
  int tC = digitalRead(triggerC);
  int tD = digitalRead(triggerD);

  if(tA == 0 && tB == 0 && tC == 0 && tD == 0)
    return 0;
  else if(tA){
    tACount++;
    tCount++;
    return 1;
  }
  else if(tB){
    tBCount++;
    tCount++;
    return 2;
  }
  else if(tC){
    tCCount++;
    tCount++;
    return 3;
  }
  else if(tD){
    tDCount++;
    tCount++;
    return 4;
  } 
  return 0;
}

int allEmpty(){
  executeAC(0,15);
  executeBD(0,15);
  return 0;
}

int executeAC(int signalPr, int timeD){
  double delayRight = timeD/3;
  double delayStraight = 2*(timeD/3);
  switch(signalPr){
    case 1:
      digitalWrite(signalA, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalC, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalA, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalC, LOW); //GO Right
      break;
    case 2:
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
int executeBD(int signalPr, int timeD){
  double delayRight = timeD/3;
  double delayStraight = 2*(timeD/3);
  switch(signalPr){
    case 3:
      digitalWrite(signalB, HIGH); //GO Right
      delay(delayRight);
      digitalWrite(signalD, HIGH); //GO Straight
      delay(delayStraight);
      digitalWrite(signalB, LOW); //GO Straight
      delay(delayRight);
      digitalWrite(signalD, LOW); //GO Right
      break;
    case 4:
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