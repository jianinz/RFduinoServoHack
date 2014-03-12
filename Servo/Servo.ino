#include <Servo.h>
#include <RFduinoBLE.h>

#define AUTOMATIC_CONTROL 1
#define MANUAL_CONTROL 2

// Led light
const int redLedPin = 2;
const int greenLedPin = 3;
const int blueLedPin = 4;

// Servo
const int servoPin = 5;
const int inserted = 140;
const int removed = 40;

// Button
const int buttonPin = 6;

int buttonState = 0;
int isConnected = 0;

Servo servo;

int currentPosition = 179;

void setup() {
  // setup the leds for output
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  pinMode(buttonPin, INPUT);

  setColor(100, 0, 0);

  //  Serial.begin(9600);
  servo.attach(servoPin, 544, 2400);
  servo.write(currentPosition);

  RFduinoBLE.advertisementInterval = 675;
  RFduinoBLE.advertisementData = "droidbot";
  RFduinoBLE.begin();
}

void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && currentPosition > 100)
    moveTo(removed);
  else if (buttonState == HIGH && currentPosition < 100)
    moveTo(inserted);
  // RFduino_ULPDelay(INFINITE);
}

void RFduinoBLE_onConnect() {
  isConnected = true;
  setColor(0, 100, 0);
}

void RFduinoBLE_onDisconnect() {
  isConnected = false;
  moveTo(removed);
  setColor(100, 0, 0);
}

void RFduinoBLE_onReceive(char *data, int len){
  int type = data[0];
  int value = data[1];

  if (type == AUTOMATIC_CONTROL) {
    if (value == 1)
      moveTo(inserted);
     else if (value == 0)
       moveTo(removed);
  } else if (type == MANUAL_CONTROL) {
    moveTo(value);
  }
}

void setColor(int red, int green, int blue) {
    analogWrite(redLedPin, red);
    analogWrite(greenLedPin, green);
    analogWrite(blueLedPin, blue);
}


// 0 is inserted and 180 removed
void moveTo(int degree) {

  setColor(100, 0, 0);

  bool forward = degree > currentPosition;

  while(degree != currentPosition) {

    if(forward)
      currentPosition++;
    else
     currentPosition--;
    servo.write(currentPosition);
    delay(5);
  }


  if (isConnected) {
    if (currentPosition > 100) {
      setColor(0, 100, 0);
    } else {
      setColor(0, 0, 100);
    }
  }
}
